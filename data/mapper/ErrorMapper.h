#pragma once

#include <string>

#include "domain/model/error/Error.h"

namespace data::mapper {
class ErrorMapper {
public:
  ErrorMapper() = default;
  ~ErrorMapper() = default;

  static domain::model::error::Error map(const std::string& message, int code);
};
} // namespace data::mapper
