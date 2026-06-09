cmake_minimum_required(VERSION 3.20)
project(testbed LANGUAGES CXX)

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

add_executable(testbed
  src/main.cpp
)

target_compile_features(testbed PRIVATE cxx_std_17)

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
