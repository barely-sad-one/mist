#include "core/logger.h"
#include "platform/platform.h"

#include <cstdarg>
#include <cstdio>

namespace mist
{

  static constexpr const char* levelPrefix[] = {
    "[FATAL] ",
    "[ERROR] ",
    "[WARN]  ",
    "[INFO]  ",
    "[DEBUG] ",
    "[TRACE] ",
  };

  static constexpr const char* levelColor[] = {
    color::BoldRed,
    color::Red,
    color::Yellow,
    color::Green,
    color::Cyan,
    color::White,
  };

  static void defaultSink(LogLevel level, const char* msg)
  {
    u8 idx = static_cast<u8>(level);

    char buf[kilobytes(4)];
    snprintf(buf, sizeof(buf), "%s%s%s%s\n",
      levelColor[idx], levelPrefix[idx], msg, color::Reset);

    if (idx <= static_cast<u8>(LogLevel::error))
      Platform::consoleWriteError(buf);
    else
      Platform::consoleWrite(buf);
  }

  static void defaultFlush() {}

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
