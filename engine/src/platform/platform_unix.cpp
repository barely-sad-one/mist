#include "platform/platform.h"

#if platform_linux || platform_unix || platform_apple

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/mman.h>

namespace mist
{

  void* Platform::allocate(usize size, Alignment align)
  {
    usize a = static_cast<usize>(align);
    usize aligned_size = (size + a - 1) & ~(a - 1);
    return aligned_alloc(a, aligned_size);
  }

  void  Platform::free(void* ptr)
  {
    ::free(ptr);
  }

  void Platform::consoleWrite(const char* msg, u8 color)
  {
    fprintf(stdout, "%s\n", msg);
  }

  void Platform::consoleWriteError(const char* msg, u8 color)
  {
    fprintf(stderr, "%s\n", msg);
  }

  void Platform::sleep(u64 ms)
  {
    timespec ts;
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
