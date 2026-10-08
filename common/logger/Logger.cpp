#include "Logger.h"

#include <chrono>
#include <iomanip>
#include <iostream>

#include "LoggerFactory.h"

namespace common::logger {

Logger logger = LoggerFactory::create();

Logger::Logger(const LogLevel logLevel)
  : m_logLevel(logLevel)
{}

void Logger::debug(const std::string& message)
{
  log(message, LogLevel::Debug);
}

void Logger::info(const std::string& message)
{
  log(message, LogLevel::Info);
}

void Logger::warning(const std::string& message)
{
  log(message, LogLevel::Warning);
}

void Logger::error(const std::string& message)
{
  log(message, LogLevel::Error);
}

void Logger::fatal(const std::string& message)
{
  log(message, LogLevel::Fatal);
}

void Logger::log(const std::string& message, const LogLevel level) const
{
  if (level < m_logLevel) {
    return;
  }

  std::string levelName;

  switch (level) {
  case LogLevel::Debug:
    levelName = "DEBUG";
    break;
  case LogLevel::Info:
    levelName = "INFO";
    break;
  case LogLevel::Warning:
    levelName = "WARNING";
    break;
  case LogLevel::Error:
    levelName = "ERROR";
    break;
  case LogLevel::Fatal:
    levelName = "FATAL";
    break;
  }

  const auto now = std::chrono::system_clock::now();

  std::cout << std::format("[{}] {:%Y-%m-%d %H:%M:%S} {}\n", levelName, now, message);
}
} // namespace common::logger
