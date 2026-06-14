#pragma once

#include "defines.h"
#include <stddef.h>

namespace mist
{

  struct PlatformState;

  enum class Alignment : usize
  {
    default_ = alignof(std::max_align_t),
    x4       = 4,
    x8       = 8,
    x16      = 16,
    x32      = 32,
    x64      = 64,
    x128     = 128,
    x256     = 256,
    x512     = 512,
    page     = 4096,
    x65536   = 65536,
  };

  class Platform
  {
    public:
      u32 mLogicalProcessorCount;
      usize mPageSize;
      static constexpr usize mArenaLargePageSize = megabytes(2);

      Platform();
      ~Platform();

      delete_copy_constructor(Platform);
      delete_move_constructor(Platform);

      MAPI static void* allocate(usize size, Alignment align = Alignment::default_);
      MAPI static void  free(void* ptr);
      MAPI static void* zero(void* ptr, usize size);
      MAPI static void* copy(void* dest, const void* src, usize size);
      MAPI static void* set(void* dest, i32 value, usize size);
      MAPI static i32   compare(const void* s1, const void* s2, usize size);
      MAPI static void  consoleWrite(const char* msg, u8 color);
      MAPI static void  consoleWriteError(const char* msg, u8 color);

      MAPI u64  getTime() const;
      MAPI void sleep(u64 ms);

      MAPI static void* reserve(usize size);
      MAPI static void* reserveLarge(usize size);
      MAPI static void  commit(void*ptr, usize size);
      MAPI static void  decommit(void*ptr, usize size);
      MAPI static void  release(void*ptr, usize size);

      MAPI f64 getSecondsElapsed() const;
      MAPI f64 getMsElapsed() const;
      MAPI u64 getStartTime() const;

      MAPI static usize stringLength(const char* str);

    private:
      PlatformState* mInternalState;
      u64 mStartTime;
  };

}
