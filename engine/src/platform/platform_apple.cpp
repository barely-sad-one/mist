#include "platform/platform.h"

#if platform_apple

#include <ctime>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>

namespace mist
{

  Platform::Platform() : mInternalState(nullptr)
  {
    mLogicalProcessorCount = static_cast<u32>(sysconf(_SC_NPROCESSORS_ONLN));
    mPageSize = get_nprocs();
    mStartTime = getTime();
  }

  Platform::~Platform() {}

  u64 Platform::getTime()
  {
    std::timespec now;
    clock_gettime(CLOCK_UPTIME_RAW, &now);
    return static_cast<u64>(now.tv_sec) * 1000000000ULL + static_cast<u64>(now.tv_nsec);
  }

  void* Platform::reserveLarge(usize size)
  {
    return reserve(size);
  }

  void  Platform::decommit(void*ptr, usize size)
  {
    mprotect(ptr, size, PROT_NONE);
    madvise(ptr, size, MADV_FREE_REUSEABLE);
  }

}

#endif
