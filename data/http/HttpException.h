#pragma once

#include <stdexcept>
#include <string>

#include "common/types.h"

namespace data::http {

class HttpException : public std::runtime_error {
public:
  HttpException(std::string message, common::types::Int16 statusCode);
  explicit HttpException(std::string message);

  [[nodiscard]] common::types::Int16 statusCode() const;

private:
  common::types::Int16 m_code = 0;
};

} // namespace data::http
