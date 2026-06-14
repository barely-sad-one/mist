#include "platform/platform.h"

#if platform_windows

#include "utils/utils.h"
#include "assertions.h"

#include <windows.h>
#include <sysinfoapi.h>
#include <malloc.h>

namespace mist
{

  struct PlatformState
  {
    usize frequency;
  };

  Platform::Platform()
    : mInternalState(nullptr)
  {
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    mLogicalProcessorCount = info.dwNumberOfProcessors;
    mPageSize = info.dwPageSize;

    mInternalState = reinterpret_cast<PlatformState*>(allocate(sizeof(PlatformState)));
    assert_trap(mInternalState != nullptr);

    LARGE_INTEGER f;
    assert_trap(QueryPerformanceFrequency(&f));
    mInternalState->frequency = f.QuadPart;

    mStartTime = getTime();
  }

  Platform::~Platform() {
    if (mInternalState)
    {
      free(reinterpret_cast<void*>(mInternalState));
      mInternalState = nullptr;
    }
  }

  void* Platform::allocate(usize size, Alignment align)
  {
    usize alignSize = alignUp(size, align);
    return _aligned_malloc(alignSize, static_cast<usize>(align));
  }

  void Platform::free(void* ptr)
  {
    _aligned_free(ptr);
  }

  void Platform::consoleWrite(const char* msg, u8 color)
  {
  }

  void Platform::consoleWriteError(const char* msg, u8 color)
  {
  }

  u64 Platform::getTime() const
  {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    u64 whole = (counter.QuadPart / mInternalState->frequency) * 1'000'000'000ULL;
    u64 frac = (counter.QuadPart % mInternalState->frequency) * 1'000'000'000ULL;
    return whole + frac;
  }

  void Platform::sleep(u64 ms)
  {
    Sleep(ms);
  }

  void* Platform::reserve(usize size)
  {
    return VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_NOACCESS);
  }

  void* Platform::reserveLarge(usize size)
  {
    return VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_LARGE_PAGES, PAGE_NOACCESS);
  }

  void Platform::commit(void* ptr, usize size)
  {
    VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
  }

  void Platform::decommit(void* ptr, usize size)
  {
    VirtualFree(ptr, size, MEM_DECOMMIT);
  }

  void Platform::release(void* ptr, usize size)
  {
    VirtualFree(ptr, 0, MEM_RELEASE);
  }

}

#endif
