#pragma once

#include "defines.h"
#include "core/logger.h"

#if compiler_clang || compiler_gcc
# if __has_builtin(__builtin_debugtrap) && defined(_DEBUG)
#   define TRAP() __builtin_debugtrap()
# elif __has_builtin(__builtin_trap)
#   define TRAP() __builtin_trap()
# else
#   warning "Compiler does not have trap support"
#   define TRAP() do { *(volatile int*)0 = 0; } while (0)
# endif

#elif compiler_msvc
# if defined(_DEBUG)
#   define TRAP() __debugbreak()
# else
#   define TRAP() __fastfail(FAST_FAIL_FATAL_APP_EXIT)
# endif

#endif


#define assert_trap(expr) \
  do { \
    if (!(expr)) { \
      TRAP(); \
    } \
  } while(0)

#define assert_trap_msg(expr, fmt, ...) \
  do { \
    if (!(expr)) { \
      log_fatal(fmt __VA_OPT__(,) __VA_ARGS__); \
      TRAP(); \
    } \
  } while(0)
