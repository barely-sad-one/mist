cmake_minimum_required(VERSION 3.20)
project(testbed LANGUAGES CXX)

add_executable(testbed
  src/main.cpp
)

target_compile_features(testbed PRIVATE cxx_std_17)

target_include_directories(testbed
  PRIVATE src
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../engine/src
)

target_link_libraries(testbed
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../engine/build/libmist.so
)
