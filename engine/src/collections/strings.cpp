#include "collections/strings.h"
#include "platform/platform.h"

namespace mist
{

  // StringView

  StringView::StringView(const char* str)
    : mData(str), mSize(str ? Platform::stringLength(str) : 0)
  {}

  StringView::StringView(const char* str, usize size)
    : mData(str), mSize(size)
  {}

  char StringView::operator[](usize idx) const
  {
    assert_trap(idx < mSize);
    return mData[idx];
  }

  const char* StringView::data()  const { return mData; }
  usize       StringView::size()  const { return mSize; }
  const char* StringView::begin() const { return mData; }
  const char* StringView::end()   const { return mData + mSize; }
  bool        StringView::empty() const { return mSize == 0; }

  bool StringView::operator==(StringView other) const
  {
    if (mSize != other.mSize) return false;
    return Platform::compare(mData, other.mData, mSize) == 0;
  }

  bool StringView::operator!=(StringView other) const { return !(*this == other); }


  // String

  String::String(Arena& arena, const char* str)
  {
    if (!str) return;
    mSize = Platform::stringLength(str);
    mData = arena.push<char>(mSize + 1);
    Platform::copy(mData, str, mSize);
    mData[mSize] = '\0';
  }

  String::String(Arena& arena, StringView str)
  {
    if (str.empty()) return;
    mSize = str.size();
    mData = arena.push<char>(mSize + 1);
    Platform::copy(mData, str.data(), mSize);
    mData[mSize] = '\0';
  }

  char String::operator[](usize idx) const
  {
    assert_trap(idx < mSize);
    return mData[idx];
  }

  const char* String::data() const
  {
    return mData;
  }

  usize String::size() const
  {
    return mSize;
  }

  const char* String::begin() const
  {
    return mData;
  }

  const char* String::end() const
  {
    return mData + mSize;
  }

  bool String::empty() const
  {
    return mSize == 0;
  }

  bool String::operator==(StringView other) const
  {
    if (mSize != other.size()) return false;
    return Platform::compare(mData, other.data(), mSize) == 0;
  }

  bool String::operator!=(StringView other) const
  {
    return !(*this == other);
  }

  // StringBuilder

  void StringBuilder::reserve(Arena& arena, usize size)
  {
    if (size <= mCapacity)
      return;
    usize newCap = mCapacity ? mCapacity * 2 : 64;
    if (newCap < size) newCap = size;
    char* newBuf = arena.push<char>(newCap);
    if (mData)
      Platform::copy(newBuf, mData, mSize);
    mData = newBuf;
    mCapacity = newCap;
  }

  void StringBuilder::append(Arena& arena, StringView str)
  {
    reserve(arena, mSize + str.size() + 1);
    Platform::copy(mData + mSize, str.data(), str.size());
    mSize       += str.size();
    mData[mSize] = '\0';
  }

  void StringBuilder::appendChar(Arena& arena, char c)
  {
    reserve(arena, mSize + 2);
    mData[mSize++] = c;
    mData[mSize]   = '\0';
  }

  String StringBuilder::build(Arena&)
  {
    String s(mData, mSize);
    mData = nullptr;
    mSize = 0;
    mCapacity = 0;
    return s;
  }

  StringView StringBuilder::view() const
  {
    return { mData, mSize };
  }
  void StringBuilder::clear()
  {
    mSize = 0;
  }

}
