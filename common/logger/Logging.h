#pragma once

#include "Logger.h"


namespace common::logger {

inline std::string formatFunctionName(std::string_view prettyFunction)
{
  std::string function{prettyFunction};

  const auto parenthesis = function.find('(');

  if (parenthesis != std::string::npos) {
    function.erase(parenthesis);
  }

  const auto space = function.rfind(' ');

  if (space != std::string::npos) {
    function.erase(0, space + 1);
  }

  const auto classStart = function.rfind("::", function.rfind("::") - 1);

  if (classStart != std::string::npos) {
    function.erase(0, classStart + 2);
  }

  return function;
}

}

#ifndef LOG_DEBUG
#define LOG_DEBUG(...) \
    common::logger::logger.debug( \
        common::logger::formatFunctionName(__PRETTY_FUNCTION__) + ": " + \
        std::format(__VA_ARGS__))
#endif

#ifndef LOG_INFO
#define LOG_INFO(...) \
    common::logger::logger.info( \
        common::logger::formatFunctionName(__PRETTY_FUNCTION__) + ": " + \
        std::format(__VA_ARGS__))
#endif

#ifndef LOG_WARNING
#define LOG_WARNING(...) \
    common::logger::logger.warning( \
        common::logger::formatFunctionName(__PRETTY_FUNCTION__) + ": " + \
        std::format(__VA_ARGS__))
#endif

#ifndef LOG_ERROR
#define LOG_ERROR(...) \
    common::logger::logger.error( \
        common::logger::formatFunctionName(__PRETTY_FUNCTION__) + ": " + \
        std::format(__VA_ARGS__))
#endif

#ifndef LOG_FATAL
#define LOG_FATAL(...) \
    common::logger::logger.fatal( \
        common::logger::formatFunctionName(__PRETTY_FUNCTION__) + ": " + \
        std::format(__VA_ARGS__))
#endif
