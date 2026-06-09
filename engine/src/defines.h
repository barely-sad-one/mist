#pragma once

#include <cstdint>
#include <cstddef>

using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
static_assert(sizeof(i8)  == 1, "Size of i8 must be 1 bytes");
static_assert(sizeof(i16) == 2, "Size of i16 must be 2 bytes");
static_assert(sizeof(i32) == 4, "Size of i32 must be 4 bytes");
static_assert(sizeof(i64) == 8, "Size of i64 must be 8 bytes");

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
static_assert(sizeof(u8)  == 1, "Size of u8 must be 1 bytes");
static_assert(sizeof(u16) == 2, "Size of u16 must be 2 bytes");
static_assert(sizeof(u32) == 4, "Size of u32 must be 4 bytes");
static_assert(sizeof(u64) == 8, "Size of u64 must be 8 bytes");

using f32 = float;
using f64 = double;
static_assert(sizeof(f32) == 4, "Size of f32 must be 4 bytes");
static_assert(sizeof(f64) == 8, "Size of f64 must be 8 bytes");

using usize = std::size_t;
using isize = std::ptrdiff_t;

using uptr = std::uintptr_t;
using iptr = std::intptr_t;


// Compiler detection
#define  compiler_msvc   0
#define  compiler_gcc    0
#define  compiler_clang  0

#if defined (__clang__)
# undef compiler_clang
# define compiler_clang 1

#elif defined (_MSC_VER)
# undef compiler_msvc
# define compiler_msvc 1

#elif defined (__GNUC__)
# undef compiler_gcc
# define compiler_gcc 1

#else
# error Compiler is currently not supported

#endif // compiler detection


// platform detection
#define  platform_windows  0
#define  platform_apple    0
#define  platform_linux    0
#define  platform_unix     0

#if defined (_WIN32) || defined (_WIN64)
# undef platform_windows
# define platform_windows 1
#endif // platform detection; windows

#if defined (__linux__)
# undef platform_linux
# define platform_linux 1
#endif // platform detection; linux

#if defined (__APPLE__) && defined (__MACH__)
# undef platform_apple
# define platform_apple 1
#endif // platform detection; apple

#if defined (__unix__) || defined (__unix)
# undef platform_unix
# define platform_unix 1
#endif // platform detection; unix

#if !(platform_windows || platform_unix || platform_apple || platform_linux)
# error Platform is not supported
#endif

// Export macor decaleration
#if defined(MEXPORT)
# if compiler_msvc
#  define MAPI __declspec(dllexport)
# elif compiler_clang || compiler_gcc
#  define MAPI __attribute__((visibility("default")))
# else
#  define MAPI
# endif
#elif defined(MIMPORT)
# if compiler_msvc
#  define MAPI __declspec(dllimport)
# else
#  define MAPI
# endif
#else
# define MAPI
#endif

// sizes macros
#define kilobytes(value) ((value) * 1024LL)
#define megabytes(value) (kilobytes(value) * 1024LL)
#define gigabytes(value) (megabytes(value) * 1024LL)

#define hundred(x)          ((x) * 100LL)
#define thousand(x)         ((x) * 1000LL)
#define hundred_thousand(x) ((x) * 100000LL)
#define million(x)          ((x) * 1000000LL)
#define billion(x)          ((x) * 1000000000LL)


// Deleting copy and move construtor
#define delete_copy_constructor(name) \
  name(const name&) = delete; \
  name& operator=(const name&) = delete

#define delete_move_constructor(name) \
  name(name&&) = delete; \
  name& operator=(name&&) = delete
