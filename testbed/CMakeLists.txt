cmake_minimum_required(VERSION 3.20)
project(testbed LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)

add_executable(testbed
  src/main.cpp
)

target_compile_features(testbed PRIVATE cxx_std_17)

target_compile_options(testbed PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
    -Wall
    -Wextra
  >

  $<$<CXX_COMPILER_ID:MSVC>:
    /W4
    /permissive-
  >

  $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU,Clang,AppleClang>>:
    -O0 -g
  >
  $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU,Clang,AppleClang>>:
    -O3
  >
  $<$<AND:$<CONFIG:RelWithDebInfo>,$<CXX_COMPILER_ID:GNU,Clang,AppleClang>>:
    -O3 -g
  >

  $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:
    /Od /Zi
  >
  $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:
    /O2
  >
  $<$<AND:$<CONFIG:RelWithDebInfo>,$<CXX_COMPILER_ID:MSVC>>:
    /O2 /Zi
  >
)

find_package(Threads REQUIRED)
target_link_libraries(testbed
  PUBLIC
    Threads::Threads
  PRIVATE
  $<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<PLATFORM_ID:Linux>>:m>
)

target_compile_definitions(testbed PRIVATE
  $<$<CONFIG:Debug>:_DEBUG>
  MIST_ARENA_FREELIST
)

target_include_directories(testbed
  PRIVATE src
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../engine/src
)

target_link_libraries(testbed
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../engine/build/$<CONFIG>/libmist.so
)
