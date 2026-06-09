#pragma once

#include "defines.h"
#include "platform/platform.h"

#include <type_traits>

namespace mist
{

  usize alignUp(usize value, Alignment align);

  template <typename T>
  struct isFlagEnum : std::false_type {};

  template <typename T, typename = std::enable_if_t<isFlagEnum<T>::value>>
  T operator|(T a, T b)
  {
    using U = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<U>(a) | static_cast<U>(b));
  }

  template <typename T, typename = std::enable_if_t<isFlagEnum<T>::value>>
  T operator&(T a, T b)
  {
    using U = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<U>(a) & static_cast<U>(b));
  }

  template <typename T, typename = std::enable_if_t<isFlagEnum<T>::value>>
  T operator~(T a)
  {
    using U = std::underlying_type_t<T>;
    return static_cast<T>(~static_cast<U>(a));
  }

  template <typename T, typename = std::enable_if_t<isFlagEnum<T>::value>>
  T& operator|=(T& a, T& b)
  {
    return a = a | b;
  }

  template <typename T, typename = std::enable_if_t<isFlagEnum<T>::value>>
  T& operator&=(T& a, T& b)
  {
    return a = a & b;
  }

#define flag_enum(T) template <> struct isFlagEnum<T> : std::true_type {}

}
