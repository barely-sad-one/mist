#include "platform/platform.h"

#if platform_linux

#include <ctime>
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <unistd.h>

namespace mist
{

  Platform::Platform() : mInternalState(nullptr)
  {
    mLogicalProcessorCount = get_nprocs();
    mPageSize = getpagesize();
    mStartTime = getTime();
  }

  Platform::~Platform() {}

  u64 Platform::getTime() const
  {
    std::timespec now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    return static_cast<u64>(now.tv_sec) * 1000000000ULL + static_cast<u64>(now.tv_nsec);
  }

  void* Platform::reserveLarge(usize size)
  {
    void* ptr = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    return (ptr == MAP_FAILED) ? nullptr : ptr;
  }

  void  Platform::decommit(void*ptr, usize size)
  {
    mprotect(ptr, size, PROT_NONE);
    madvise(ptr, size, MADV_DONTNEED);
  }

}

#endif
