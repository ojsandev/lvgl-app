#pragma once

#include <string>

namespace domain::model {
enum class IOResultType { Success = 0, Failure = 1 };

struct IOResult
{
  IOResultType type;
  std::string message;
};
} // namespace domain::model
