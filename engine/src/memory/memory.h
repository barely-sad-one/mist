#pragma once

#include "defines.h"
#include "platform/platform.h"
#include <array>
#include <atomic>

namespace mist
{

#define X_MEMORY_TYPE \
    _X_MEMORY_TYPE(unknown) \
    _X_MEMORY_TYPE(arena) \
    _X_MEMORY_TYPE(platform) \
    _X_MEMORY_TYPE(application) \
    _X_MEMORY_TYPE(renderer)

  enum class MemoryType : u8
  {
#define _X_MEMORY_TYPE(value) value,
    X_MEMORY_TYPE
#undef _X_MEMORY_TYPE

    count
  };

  class MAPI Memory {
    public:
      Memory();
      ~Memory();

      void* allocate(MemoryType type, usize size, Alignment align = Alignment::default_);

      template <typename T>
      T* allocate(MemoryType type, usize count, Alignment align = Alignment::default_)
      {
        return static_cast<T*>(allocate(type, sizeof(T) * count, align));
      }

      void free(MemoryType type, void* ptr, usize size, Alignment align = Alignment::default_);

      template <typename T>
      void free(MemoryType type, T* ptr, usize count, Alignment align = Alignment::default_)
      {
        free(type, static_cast<void*>(ptr), sizeof(T) * count, align);
      }

      void* reserve(MemoryType type, usize size);
      void* reserveLarge(MemoryType type, usize size);
      void  commit(void* ptr, usize size);
      void  decommit(void* ptr, usize size);
      void  release(MemoryType type, void* ptr, usize size);

      static void* copy(void* dest, const void* src, usize size);

      template <typename T>
      static T* copy(T* dest, const T* src, usize count)
      {
        return reinterpret_cast<T*>(copy(dest, src, sizeof(T) * count));
      }

      void logStats() const;

    private:
      void addAllocation(MemoryType type, usize size);
      void subAllocation(MemoryType type, usize size);

      std::atomic<usize> mTotalAllocated;
      std::array<std::atomic<usize> , static_cast<usize>(MemoryType::count)> mAllocated;
  };

}
