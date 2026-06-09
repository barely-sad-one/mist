cmake_minimum_required(VERSION 3.20)
project(mist LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

if(MSVC)
  set(CMAKE_CXX_FLAGS_DEBUG          "/Od /Zi" CACHE STRING "" FORCE)
  set(CMAKE_CXX_FLAGS_RELEASE        "/Od"     CACHE STRING "" FORCE)
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Od /Zi" CACHE STRING "" FORCE)
else()
  set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g"  CACHE STRING "" FORCE)
  set(CMAKE_CXX_FLAGS_RELEASE        "-O0"     CACHE STRING "" FORCE)
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O0 -g"  CACHE STRING "" FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)

add_library(mist SHARED
  src/core/logger.cpp
  src/core/application.cpp
  src/platform/platform.cpp
  src/platform/platform_unix.cpp
  src/platform/platform_linux.cpp
  src/platform/platform_apple.cpp
  src/platform/platform_windows.cpp
  src/memory/memory.cpp
  src/memory/allocator.cpp
  src/utils/utils.cpp
  src/collections/strings.cpp
)

target_compile_features(mist PUBLIC cxx_std_17)

target_compile_definitions(mist
  PRIVATE
    MEXPORT
    $<$<CONFIG:Debug>:_DEBUG>
  PUBLIC
    MIST_ARENA_FREELIST
)

target_include_directories(mist
  PUBLIC src
)
