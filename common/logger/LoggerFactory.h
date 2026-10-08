#pragma once

#include "Logger.h"

namespace common::logger {
class LoggerFactory {
public:
  static Logger create();
};
} // namespace common::logger
