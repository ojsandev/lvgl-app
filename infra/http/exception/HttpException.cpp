#include "HttpException.h"

#include <utility>

using namespace infra::http::exception;

HttpException::HttpException(std::string message, const common::types::Int16 statusCode)
  : runtime_error(message),
    m_message(std::move(message)),
    m_code(statusCode) {}

HttpException::HttpException(std::string message) : runtime_error(message), m_message(std::move(message)) {}

const char* HttpException::what() const noexcept
{
  return m_message.c_str();
}

common::types::Int16 HttpException::statusCode() const
{
  return m_code;
}
