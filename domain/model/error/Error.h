#pragma once

#include "common/types.h"

namespace domain::model::error {

enum class Reason {
  Unknown = 0,
  NotFound = 1,
  InvalidInput = 2,
  Unauthorized = 3,
  Forbidden = 4,
  Conflict = 5,
  InternalServerError = 6
};

struct Error
{
  Reason reason;
  std::string message;
};
} // namespace domain::model::error
