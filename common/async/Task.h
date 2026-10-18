#pragma once

#include <exception>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#if defined(__cpp_impl_coroutine) && __cpp_impl_coroutine >= 201902L && defined(__has_include)
#if __has_include(<coroutine>)
#include <coroutine>
#define COMMON_ASYNC_HAS_COROUTINES 1
#else
#define COMMON_ASYNC_HAS_COROUTINES 0
#endif
#else
#define COMMON_ASYNC_HAS_COROUTINES 0
#endif

namespace common::async::detail {

template <typename T> struct TaskState
{
  mutable std::mutex mutex;
  std::optional<T> value;
  std::exception_ptr exception;
  bool ready = false;
  bool cancellationRequested = false;
#if COMMON_ASYNC_HAS_COROUTINES
  std::vector<std::coroutine_handle<>> continuations;
#endif
};

#if COMMON_ASYNC_HAS_COROUTINES
inline void resumeContinuations(std::vector<std::coroutine_handle<>> continuations)
{
  for (auto continuation : continuations) {
    if (continuation) {
      continuation.resume();
    }
  }
}
#endif

template <typename T, typename U> bool tryCompleteValue(const std::shared_ptr<TaskState<T>>& state, U&& value)
{
#if COMMON_ASYNC_HAS_COROUTINES
  std::vector<std::coroutine_handle<>> continuations;
#endif
  {
    std::lock_guard lock(state->mutex);
    if (state->ready) {
      return false;
    }
    state->value.emplace(std::forward<U>(value));
    state->ready = true;
#if COMMON_ASYNC_HAS_COROUTINES
    continuations = std::move(state->continuations);
#endif
  }
#if COMMON_ASYNC_HAS_COROUTINES
  resumeContinuations(std::move(continuations));
#endif
  return true;
}

template <typename T>
bool tryCompleteException(const std::shared_ptr<TaskState<T>>& state, std::exception_ptr exception)
{
#if COMMON_ASYNC_HAS_COROUTINES
  std::vector<std::coroutine_handle<>> continuations;
#endif
  {
    std::lock_guard lock(state->mutex);
    if (state->ready) {
      return false;
    }
    state->exception =
        exception ? exception : std::make_exception_ptr(std::runtime_error("Task failed without an exception"));
    state->ready = true;
#if COMMON_ASYNC_HAS_COROUTINES
    continuations = std::move(state->continuations);
#endif
  }
#if COMMON_ASYNC_HAS_COROUTINES
  resumeContinuations(std::move(continuations));
#endif
  return true;
}

template <typename T> bool tryCancel(const std::shared_ptr<TaskState<T>>& state, std::exception_ptr exception)
{
#if COMMON_ASYNC_HAS_COROUTINES
  std::vector<std::coroutine_handle<>> continuations;
#endif
  {
    std::lock_guard lock(state->mutex);
    if (state->ready) {
      return false;
    }
    state->cancellationRequested = true;
    state->exception =
        exception ? std::move(exception) : std::make_exception_ptr(std::runtime_error("Task was cancelled"));
    state->ready = true;
#if COMMON_ASYNC_HAS_COROUTINES
    continuations = std::move(state->continuations);
#endif
  }
#if COMMON_ASYNC_HAS_COROUTINES
  resumeContinuations(std::move(continuations));
#endif
  return true;
}

template <typename T, typename U> void completeValue(const std::shared_ptr<TaskState<T>>& state, U&& value)
{
  if (!tryCompleteValue<T>(state, std::forward<U>(value))) {
    throw std::logic_error("Task completed more than once");
  }
}

template <typename T> void completeException(const std::shared_ptr<TaskState<T>>& state, std::exception_ptr exception)
{
  if (!tryCompleteException<T>(state, std::move(exception))) {
    throw std::logic_error("Task completed more than once");
  }
}

} // namespace common::async::detail

namespace common::async {

class TaskCanceledException : public std::runtime_error {
public:
  TaskCanceledException()
    : std::runtime_error("Task was cancelled")
  {}
};

template <typename T> class TaskCompletionSource;

template <typename T> class Task {
public:
  Task() = delete;

  [[nodiscard]] bool isReady() const noexcept
  {
    std::lock_guard lock(m_state->mutex);
    return m_state->ready;
  }

  [[nodiscard]] bool isCancellationRequested() const noexcept
  {
    std::lock_guard lock(m_state->mutex);
    return m_state->cancellationRequested;
  }

  [[nodiscard]] bool isCanceled() const noexcept
  {
    std::lock_guard lock(m_state->mutex);
    return m_state->ready && m_state->cancellationRequested;
  }

  bool requestCancel() const noexcept
  {
    return detail::tryCancel<T>(m_state, std::make_exception_ptr(TaskCanceledException{}));
  }

  [[nodiscard]] bool hasError() const
  {
    std::lock_guard lock(m_state->mutex);
    if (!m_state->ready) {
      throw std::logic_error("Task is not ready");
    }
    return static_cast<bool>(m_state->exception);
  }

  [[nodiscard]] std::exception_ptr error() const
  {
    std::lock_guard lock(m_state->mutex);
    if (!m_state->ready) {
      throw std::logic_error("Task is not ready");
    }
    return m_state->exception;
  }

  void rethrowError() const
  {
    const auto exception = error();
    if (exception) {
      std::rethrow_exception(exception);
    }
  }

  const T& result() const
  {
    std::lock_guard lock(m_state->mutex);
    if (!m_state->ready) {
      throw std::logic_error("Task result requested before completion");
    }
    if (m_state->exception) {
      std::rethrow_exception(m_state->exception);
    }
    return *m_state->value;
  }

#if COMMON_ASYNC_HAS_COROUTINES
  struct Awaiter
  {
    std::shared_ptr<detail::TaskState<T>> state;

    bool await_ready() const noexcept
    {
      std::lock_guard lock(state->mutex);
      return state->ready;
    }

    bool await_suspend(std::coroutine_handle<> continuation)
    {
      std::lock_guard lock(state->mutex);
      if (state->ready) {
        return false;
      }
      state->continuations.push_back(continuation);
      return true;
    }

    T await_resume() const
    {
      std::lock_guard lock(state->mutex);
      if (!state->ready) {
        throw std::logic_error("Task resumed before completion");
      }
      if (state->exception) {
        std::rethrow_exception(state->exception);
      }
      return *state->value;
    }
  };

  [[nodiscard]] Awaiter operator co_await() const
  {
    return Awaiter{ m_state };
  }

  struct promise_type
  {
    std::shared_ptr<detail::TaskState<T>> state = std::make_shared<detail::TaskState<T>>();

    Task get_return_object()
    {
      return Task(state);
    }

    std::suspend_never initial_suspend() const noexcept
    {
      return {};
    }
    std::suspend_never final_suspend() const noexcept
    {
      return {};
    }

    template <typename U> void return_value(U&& value)
    {
      detail::tryCompleteValue<T>(state, std::forward<U>(value));
    }

    void unhandled_exception()
    {
      detail::tryCompleteException<T>(state, std::current_exception());
    }
  };
#endif

private:
  explicit Task(std::shared_ptr<detail::TaskState<T>> state)
    : m_state(std::move(state))
  {}

  std::shared_ptr<detail::TaskState<T>> m_state;

  friend class TaskCompletionSource<T>;
};

template <typename T> class TaskCompletionSource {
public:
  TaskCompletionSource()
    : m_state(std::make_shared<detail::TaskState<T>>())
  {}

  [[nodiscard]] Task<T> task() const
  {
    return Task<T>(m_state);
  }

  void set_value(T value) const
  {
    detail::completeValue<T>(m_state, std::move(value));
  }

  [[nodiscard]] bool try_set_value(T value) const
  {
    return detail::tryCompleteValue<T>(m_state, std::move(value));
  }

  void set_exception(std::exception_ptr exception) const
  {
    detail::completeException<T>(m_state, std::move(exception));
  }

  [[nodiscard]] bool try_set_exception(std::exception_ptr exception) const
  {
    return detail::tryCompleteException<T>(m_state, std::move(exception));
  }

  bool set_canceled() const
  {
    return detail::tryCancel<T>(m_state, std::make_exception_ptr(TaskCanceledException{}));
  }

  [[nodiscard]] bool isCancellationRequested() const noexcept
  {
    std::lock_guard lock(m_state->mutex);
    return m_state->cancellationRequested;
  }

private:
  std::shared_ptr<detail::TaskState<T>> m_state;
};

} // namespace common::async
