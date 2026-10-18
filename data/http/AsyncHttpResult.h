#pragma once

#include <exception>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "common/types.h"

namespace data::http {

template <typename T>
class AsyncHttpResult
{
public:
  AsyncHttpResult(std::optional<T> value, std::exception_ptr error);

  static AsyncHttpResult success(T value);
  static AsyncHttpResult failure(std::exception_ptr error);
  [[nodiscard]] bool hasValue() const noexcept;
  explicit operator bool() const noexcept;
  const T& value() const &;
  T&& value() &&;
  [[nodiscard]] std::exception_ptr error() const noexcept;
  void rethrowError() const;

private:
  std::optional<T> m_value;
  std::exception_ptr m_error;
};

using AsyncHttpStringResult = AsyncHttpResult<std::string>;
using AsyncHttpBytesResult = AsyncHttpResult<std::vector<common::types::UInt8>>;

template <typename T>
AsyncHttpResult<T>::AsyncHttpResult(std::optional<T> value, std::exception_ptr error)
  : m_value(std::move(value))
    , m_error(std::move(error)) {}

template <typename T>
AsyncHttpResult<T> AsyncHttpResult<T>::success(T value)
{
  return AsyncHttpResult(std::optional<T>{std::move(value)}, nullptr);
}

template <typename T>
AsyncHttpResult<T> AsyncHttpResult<T>::failure(std::exception_ptr error)
{
  return AsyncHttpResult(std::nullopt, std::move(error));
}

template <typename T>
bool AsyncHttpResult<T>::hasValue() const noexcept
{
  return m_value.has_value();
}

template <typename T>
AsyncHttpResult<T>::operator bool() const noexcept
{
  return hasValue();
}

template <typename T>
const T& AsyncHttpResult<T>::value() const &
{
  if (!m_value) {
    rethrowError();
    throw std::logic_error("Async HTTP result has no value");
  }
  return *m_value;
}

template <typename T>
T&& AsyncHttpResult<T>::value() &&
{
  if (!m_value) {
    rethrowError();
    throw std::logic_error("Async HTTP result has no value");
  }
  return std::move(*m_value);
}

template <typename T>
std::exception_ptr AsyncHttpResult<T>::error() const noexcept
{
  return m_error;
}

template <typename T>
void AsyncHttpResult<T>::rethrowError() const
{
  if (m_error) {
    std::rethrow_exception(m_error);
  }
}

} // namespace data::http
