#include "platform/platform.h"

#include <string.h>

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
    return strlen(str);
  }

  void* Platform::zero(void* ptr, usize size)
  {
    return memset(ptr, 0, size);
  }

  void* Platform::copy(void* dest, const void* src, usize size)
  {
    return memcpy(dest, src, size);
  }

  void* Platform::set(void* dest, i32 value, usize size)
  {
    return memset(dest, value, size);
  }

  i32 Platform::compare(const void* s1, const void* s2, usize size)
  {
    return memcmp(s1, s2, size);
  }
}
