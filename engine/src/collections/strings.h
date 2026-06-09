#pragma once

#include "defines.h"
#include "assertions.h"
#include "platform/platform.h"
#include "memory/allocator.h"

namespace mist
{

  struct MAPI StringView
  {
    StringView() = default;

    StringView(const char* str);

    StringView(const char* str, usize size);

    char operator[](usize idx) const;
    bool operator==(StringView other) const;
    bool operator!=(StringView other) const;

    const char* data() const;
    usize size() const;

    const char* begin() const;
    const char* end() const;
    bool empty() const;

    private:
      const char* mData = nullptr;
      usize mSize = 0;
  };

  struct MAPI String
  {
    String() = default;

    String(Arena& arena, const char* str);
    String(Arena& arena, StringView str);

    char operator[](usize idx) const;
    bool operator==(String other) const;
    bool operator!=(String other) const;

    const char* data() const;
    usize size() const;

    const char* begin() const;
    const char* end() const;
    bool empty() const;

    private:
      char* mData = nullptr;
      usize mSize = 0;
  };


  struct MAPI StringBuilder
  {
    StringBuilder() = default;

    void append(Arena& arena, StringView str);
    void appendChar(Arena& arena, char c);

    String build(Arena& arena);
    StringView view() const;
    void clear();
    void reserve(Arena& arena, usize size);

    private:
      char* mData = nullptr;
      usize mSize = 0;
      usize mCapacity = 0;
  };

}
