#include "core/logger.h"

#include <cstdarg>
#include <cstdio>

namespace mist
{

  static const char* levelPrefix[] = {
    "[FATAL] ",
    "[ERROR] ",
    "[WARN]  ",
    "[INFO]  ",
    "[DEBUG] ",
    "[TRACE] ",
  };

#if !platform_windows
  static const char* levelColor[] = {
    "\033[1;31m",
    "\033[0;31m",
    "\033[0;33m",
    "\033[0;32m",
    "\033[0;36m",
    "\033[0;37m",
  };
  static const char* colorReset = "\033[0m";
#endif

  static void defaultSink(LogLevel level, const char* msg)
  {
    u8 idx = static_cast<u8>(level);
    if (idx <= static_cast<u8>(LogLevel::error))
    {
#if platform_windows
      fprintf(stderr, "%s%s\n", levelPrefix[idx], msg);
#else
      fprintf(stderr, "%s%s%s%s\n", levelColor[idx], levelPrefix[idx], msg, colorReset);
#endif
    } else
    {
#if platform_windows
      fprintf(stdout, "%s%s\n", levelPrefix[idx], msg);
#else
      fprintf(stdout, "%s%s%s%s\n", levelColor[idx], levelPrefix[idx], msg, colorReset);
#endif
    }
  }

  static void defaultFlush()
  {
    fflush(stdout);
    fflush(stderr);
  }

  Logger::Logger() : mSink(defaultSink), mFlush(defaultFlush) {}

  Logger::Logger(LogFnT logFunc, LogFlushFnT flushFunc)
    : mSink(logFunc), mFlush(flushFunc) {}

  Logger::~Logger()
  {
    if (mFlush) mFlush();
  }

  void Logger::log(LogLevel level, const char* fmt, ...)
  {
    char buf[kilobytes(4)];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    mSink(level, buf);
  }

  void Logger::setLogger(LogFnT logFunc, LogFlushFnT flushFunc)
  {
    if (mFlush) mFlush();
    mSink = logFunc;
    mFlush = flushFunc;
  }

}
