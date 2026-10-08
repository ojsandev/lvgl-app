#include "LoggerFactory.h"

using namespace common::logger;

Logger LoggerFactory::create()
{

#ifdef LOGGER_DEBUG
  return Logger{ LogLevel::Debug };
  ;
#else
  return Logger{ LogLevel::Info };
#endif
}
