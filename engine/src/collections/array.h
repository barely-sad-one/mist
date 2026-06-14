#pragma once

#include "defines.h"
#include "memory/memory.h"
#include "memory/allocator.h"
#include <core/application.h>
#include "platform/platform.h"
#include "assertions.h"

#include <type_traits>
#include <array>

namespace mist
{

  template <typename T, usize N>
  using FixedArray = std::array<T, N>;

  template <typename T>
  struct Array
  {
    static_assert(std::is_trivially_copyable_v<T>, "Array only supports trivially copyable types");

    T& operator[](usize i)
    {
      assert_trap(i < mCount);
      return mData[i];
    }

    const T& operator[](usize i) const
    {
      assert_trap(i < mCount);
      return mData[i];
    }

    T* begin()
    {
      return mData;
    }

    T* end()
    {
      return mData + mCount;
    }

    const T* begin() const
    {
      return mData;
    }

    const T* end() const
    {
      return mData + mCount;
    }

    usize count() const
    {
      return mCount;
    }

    usize capacity() const
    {
      return mCapacity;
    }

    bool empty() const
    {
      return mCount == 0;
    }

    void reserve(Arena& arena, usize n)
    {
      if (n <= mCapacity)
        return;
      T* newData = arena.push<T>(n);
      if (mData)
        mist_memory.copy(newData, mData, sizeof(T) * mCount);
      mData = newData;
      mCapacity = n;
    }

    void push(Arena& arena, const T& val)
    {
      if (mCount >= mCapacity)
        grow(arena);
      mData[mCount++] = val;
    }

    void push(Arena& arena, T&& val)
    {
      if (mCount >= mCapacity)
        grow(arena);
      mData[mCount++] = std::move(val);
    }

    T* push(Arena& arena)
    {
      if (mCount >= mCapacity)
        grow(arena);
      return &mData[mCount++];
    }

    void pop()
    {
      assert_trap(mCount > 0);
      --mCount;
    }

    void removeAtUnordered(usize i)
    {
      assert_trap(i < mCount);
      mData[i] = mData[--mCount];
    }

    void clear() { mCount = 0; }

    private:
      void grow(Arena& arena)
      {
        reserve(arena, mCapacity ? mCapacity * 2 : 8);
      }

      T*    mData     = nullptr;
      usize mCount    = 0;
      usize mCapacity = 0;
  };

}
