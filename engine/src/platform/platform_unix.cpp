#include "platform/platform.h"

#if platform_linux || platform_unix || platform_apple

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <sys/mman.h>

namespace mist
{

  Platform::~Platform() {}

  void* Platform::allocate(usize size, Alignment align)
  {
    usize a = static_cast<usize>(align);
    usize aligned_size = (size + a - 1) & ~(a - 1);
    return std::aligned_alloc(a, aligned_size);
  }

  void  Platform::free(void* ptr)
  {
    std::free(ptr);
  }

  void* Platform::zero(void* ptr, usize size)
  {
    return std::memset(ptr, 0, size);
  }

  void* Platform::copy(void* dest, const void* src, usize size)
  {
    return std::memcpy(dest, src, size);
  }

  void* Platform::set(void* dest, i32 value, usize size)
  {
    return std::memset(dest, value, size);
  }

  i32 Platform::compare(const void* s1, const void* s2, usize size)
  {
    return std::memcmp(s1, s2, size);
  }

  void Platform::consoleWrite(const char* msg, u8 color)
  {
    std::fprintf(stdout, "%s\n", msg);
  }

  void Platform::consoleWriteError(const char* msg, u8 color)
  {
    std::fprintf(stderr, "%s\n", msg);
  }

  void Platform::sleep(u64 ms)
  {
    std::timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, 0);
  }

  void* Platform::reserve(usize size)
  {
    void* ptr = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return (ptr == MAP_FAILED) ? nullptr : ptr;
  }

  void  Platform::commit(void*ptr, usize size)
  {
    mprotect(ptr, size, PROT_READ | PROT_WRITE);
  }

  void  Platform::release(void*ptr, usize size)
  {
    munmap(ptr, size);
  }

}

#endif
