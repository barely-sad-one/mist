#include "memory/memory.h"
#include "core/logger.h"
#include "core/application.h"
#include "utils/utils.h"

namespace mist
{
  static const char* memoryTypeStrings[static_cast<usize>(MemoryType::count)] = {
#define _X_MEMORY_TYPE(value) #value,
    X_MEMORY_TYPE
#undef _X_MEMORY_TYPE
  };

  Memory::Memory()
  {
    mTotalAllocated.store(0);
    for (std::atomic<usize>& a : mAllocated)
    {
      a.store(0, std::memory_order_relaxed);
    }
  }

  Memory::~Memory()
  {
  }

  void* Memory::allocate(MemoryType type, usize size, Alignment align)
  {
#if defined (_DEBUG)
    addAllocation(type, alignUp(size, align));
#endif
    return Platform::allocate(size, align);
  }

  void Memory::free(MemoryType type, void* ptr, usize size, Alignment align)
  {
#if defined (_DEBUG)
    subAllocation(type, alignUp(size, align));
#endif
    Platform::free(ptr);
  }

  void Memory::addAllocation(MemoryType type, usize size)
  {
    mAllocated[static_cast<usize>(type)].fetch_add(size, std::memory_order_relaxed);
    mTotalAllocated.fetch_add(size, std::memory_order_relaxed);
  }

  void Memory::subAllocation(MemoryType type, usize size)
  {
    mAllocated[static_cast<usize>(type)].fetch_sub(size, std::memory_order_relaxed);
    mTotalAllocated.fetch_sub(size, std::memory_order_relaxed);
  }

  void Memory::logStats() const
  {
    log_info("Memory Allocation Stats:");
    log_info("Total Allocation: %lld", mTotalAllocated.load(std::memory_order_relaxed));
    for (u8 i = 0; i < static_cast<usize>(MemoryType::count); ++i)
    {
      log_info("[%s]: %lld", memoryTypeStrings[i], mAllocated[i].load(std::memory_order_relaxed));
    }
  }

  void* Memory::reserve(MemoryType type, usize size)
  {
#if defined (_DEBUG)
    addAllocation(type, size);
#endif
    return Platform::reserve(size);
  }

  void* Memory::reserveLarge(MemoryType type, usize size)
  {
#if defined (_DEBUG)
    addAllocation(type, size);
#endif
    return Platform::reserveLarge(size);
  }

  void  Memory::commit(void* ptr, usize size)
  {
    Platform::commit(ptr, size);
  }

  void  Memory::decommit(void* ptr, usize size)
  {
    Platform::decommit(ptr, size);
  }

  void  Memory::release(MemoryType type, void* ptr, usize size)
  {
#if defined (_DEBUG)
    subAllocation(type, size);
#endif
    Platform::release(ptr, size);
  }

  void* Memory::copy(void* dest, const void* src, usize size)
  {
    return Platform::copy(dest, src, size);
  }

}
