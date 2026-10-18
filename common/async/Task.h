#pragma once

#include <exception>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>

#if defined(__cpp_impl_coroutine) && __cpp_impl_coroutine >= 201902L
#include <coroutine>
#define COMMON_ASYNC_HAS_COROUTINES 1
#else
#define COMMON_ASYNC_HAS_COROUTINES 0
#endif

namespace common::async::detail {

template <typename T>
struct TaskState {
  mutable std::mutex mutex;
  std::optional<T> value;
  std::exception_ptr exception;
  bool ready = false;
#if COMMON_ASYNC_HAS_COROUTINES
  std::coroutine_handle<> continuation{};
#endif
};

template <typename T, typename U>
void completeValue(const std::shared_ptr<TaskState<T>>& state, U&& value)
{
#if COMMON_ASYNC_HAS_COROUTINES
  std::coroutine_handle<> continuation;
#endif
  {
    std::lock_guard lock(state->mutex);
    if (state->ready) {
      throw std::logic_error("Task completed more than once");
    }
    state->value.emplace(std::forward<U>(value));
    state->ready = true;
#if COMMON_ASYNC_HAS_COROUTINES
    continuation = std::exchange(state->continuation, {});
#endif
  }
#if COMMON_ASYNC_HAS_COROUTINES
  if (continuation) {
    continuation.resume();
  }
#endif
}

template <typename T>
void completeException(const std::shared_ptr<TaskState<T>>& state, std::exception_ptr exception)
{
#if COMMON_ASYNC_HAS_COROUTINES
  std::coroutine_handle<> continuation;
#endif
  {
    std::lock_guard lock(state->mutex);
    if (state->ready) {
      throw std::logic_error("Task completed more than once");
    }
    state->exception = exception ? exception : std::make_exception_ptr(
      std::runtime_error("Task failed without an exception"));
    state->ready = true;
#if COMMON_ASYNC_HAS_COROUTINES
    continuation = std::exchange(state->continuation, {});
#endif
  }
#if COMMON_ASYNC_HAS_COROUTINES
  if (continuation) {
    continuation.resume();
  }
#endif
}

} // namespace common::async::detail

namespace common::async {

template <typename T>
class TaskCompletionSource;

template <typename T>
class Task {
public:
  Task() = delete;

  [[nodiscard]] bool isReady() const noexcept
  {
    std::lock_guard lock(m_state->mutex);
    return m_state->ready;
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
  struct Awaiter {
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
      if (state->continuation) {
        throw std::logic_error("Task supports only one awaiting coroutine");
      }
      state->continuation = continuation;
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
    return Awaiter{m_state};
  }

  struct promise_type {
    std::shared_ptr<detail::TaskState<T>> state =
      std::make_shared<detail::TaskState<T>>();

    Task get_return_object()
    {
      return Task(state);
    }

    std::suspend_never initial_suspend() const noexcept { return {}; }
    std::suspend_never final_suspend() const noexcept { return {}; }

    template <typename U>
    void return_value(U&& value)
    {
      detail::completeValue<T>(state, std::forward<U>(value));
    }

    void unhandled_exception()
    {
      detail::completeException<T>(state, std::current_exception());
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

template <typename T>
class TaskCompletionSource {
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

  void set_exception(std::exception_ptr exception) const
  {
    detail::completeException<T>(m_state, std::move(exception));
  }

private:
  std::shared_ptr<detail::TaskState<T>> m_state;
};

} // namespace common::async
