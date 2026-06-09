#pragma once

#include "defines.h"

namespace mist
{

  class Application;

#define X_LOG_LEVEL \
  _X_LOG_LEVEL(fatal) \
  _X_LOG_LEVEL(error) \
  _X_LOG_LEVEL(warn)  \
  _X_LOG_LEVEL(info)  \
  _X_LOG_LEVEL(debug) \
  _X_LOG_LEVEL(trace) \

  enum class LogLevel : u8 {
#define _X_LOG_LEVEL(level) level,
    X_LOG_LEVEL
#undef _X_LOG_LEVEL
  };

  class MAPI Logger
  {
    public:
      using LogFnT = void(*)(LogLevel level, const char* fmt);
      using LogFlushFnT = void(*)();

      Logger();
      Logger(LogFnT logFunc, LogFlushFnT flushFunc);
      ~Logger();

      void log(LogLevel level, const char* fmt, ...);
      void setLogger(LogFnT logFunc, LogFlushFnT flushFunc);

    private:
      LogFnT mSink;
      LogFlushFnT mFlush;
  };

}

#define log_fatal(fmt, ...) mist_application.logger.log(mist::LogLevel::fatal, fmt __VA_OPT__(,) __VA_ARGS__)
#define log_error(fmt, ...) mist_application.logger.log(mist::LogLevel::error, fmt __VA_OPT__(,) __VA_ARGS__)
#define log_warn(fmt, ...)  mist_application.logger.log(mist::LogLevel::warn,  fmt __VA_OPT__(,) __VA_ARGS__)
#define log_info(fmt, ...)  mist_application.logger.log(mist::LogLevel::info,  fmt __VA_OPT__(,) __VA_ARGS__)
#define log_debug(fmt, ...) mist_application.logger.log(mist::LogLevel::debug, fmt __VA_OPT__(,) __VA_ARGS__)
#define log_trace(fmt, ...) mist_application.logger.log(mist::LogLevel::trace, fmt __VA_OPT__(,) __VA_ARGS__)
