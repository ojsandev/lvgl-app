#include "data/http/HttpException.h"

#include <utility>

using namespace data::http;

HttpException::HttpException(std::string message, const common::types::Int16 statusCode)
  : runtime_error(std::move(message))
  , m_code(statusCode)
{}

HttpException::HttpException(std::string message)
  : runtime_error(std::move(message))
{}

common::types::Int16 HttpException::statusCode() const
{
  return m_code;
}
