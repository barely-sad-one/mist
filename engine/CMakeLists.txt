cmake_minimum_required(VERSION 3.20)
project(mist LANGUAGES CXX)

add_library(mist SHARED
  src/core/test.cpp
)

target_compile_features(mist PUBLIC cxx_std_17)
target_compile_definitions(mist
  PRIVATE MEXPORT
)

target_include_directories(mist
  PUBLIC src
)


