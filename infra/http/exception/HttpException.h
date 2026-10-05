#pragma once

#include "common/types.h"
#include <string>
#include <stdexcept>

namespace infra::http::exception {
class HttpException : public std::runtime_error {
public:
  HttpException(std::string message, common::types::Int16 statusCode);
  explicit HttpException(std::string message);


  [[nodiscard]] const char* what() const noexcept override;

  [[nodiscard]] common::types::Int16 statusCode() const;

private:
  std::string m_message;
  common::types::Int16 m_code;
};
}
