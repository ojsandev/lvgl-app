#pragma once

#include <exception>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "common/types.h"

namespace data::http {

template <typename T>
class AsyncHttpResult {
public:
  static AsyncHttpResult success(T value)
  {
    return AsyncHttpResult(std::optional<T>{std::move(value)}, nullptr);
  }

  static AsyncHttpResult failure(std::exception_ptr error)
  {
    return AsyncHttpResult(std::nullopt, std::move(error));
  }

  [[nodiscard]] bool hasValue() const noexcept { return m_value.has_value(); }
  [[nodiscard]] explicit operator bool() const noexcept { return hasValue(); }

  [[nodiscard]] const T& value() const&
  {
    if (!m_value) {
      rethrowError();
      throw std::logic_error("Async HTTP result has no value");
    }
    return *m_value;
  }

  [[nodiscard]] T&& value() &&
  {
    if (!m_value) {
      rethrowError();
      throw std::logic_error("Async HTTP result has no value");
    }
    return std::move(*m_value);
  }

  [[nodiscard]] std::exception_ptr error() const noexcept { return m_error; }

  void rethrowError() const
  {
    if (m_error) {
      std::rethrow_exception(m_error);
    }
  }

private:
  AsyncHttpResult(std::optional<T> value, std::exception_ptr error)
    : m_value(std::move(value))
    , m_error(std::move(error))
  {}

  std::optional<T> m_value;
  std::exception_ptr m_error;
};

using AsyncHttpStringResult = AsyncHttpResult<std::string>;
using AsyncHttpBytesResult = AsyncHttpResult<std::vector<common::types::UInt8>>;

} // namespace data::http
