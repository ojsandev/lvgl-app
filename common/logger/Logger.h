#pragma once

#include "ILogger.h"

namespace common::logger {

class Logger : public ILogger {
public:
  explicit Logger(LogLevel logLevel = LogLevel::Info);
  ~Logger() override = default;

  void debug(const std::string& message) override;
  void info(const std::string& message) override;
  void warning(const std::string& message) override;
  void error(const std::string& message) override;
  void fatal(const std::string& message) override;

private:
  void log(const std::string& message, LogLevel level) const;

  LogLevel m_logLevel;
};

extern Logger logger;

} // namespace common::logger
