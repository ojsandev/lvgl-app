#pragma once

#include <format>
#include <string>

namespace common::logger {
enum class LogLevel { Debug = 1, Info = 2, Warning = 3, Error = 4, Fatal = 5 };

class ILogger {
public:
  virtual ~ILogger() = default;

  virtual void debug(const std::string& message) = 0;
  virtual void info(const std::string& message) = 0;
  virtual void warning(const std::string& message) = 0;
  virtual void error(const std::string& message) = 0;
  virtual void fatal(const std::string& message) = 0;
};
} // namespace common::logger
