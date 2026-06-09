#include "platform/platform.h"

#include <cstring>

namespace mist
{
  f64 Platform::getSecondsElapsed() const
  {
    return static_cast<f64>(getTime() - mStartTime) / 1e9;
  }

  f64 Platform::getMsElapsed() const
  {
    return static_cast<f64>(getTime() - mStartTime) / 1e6;
  }

  u64 Platform::getStartTime() const
  {
    return mStartTime;
  }

  usize Platform::stringLength(const char* str)
  {
    return std::strlen(str);
  }
}
