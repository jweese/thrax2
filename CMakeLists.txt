cmake_minimum_required(VERSION 3.9)
project(thrax2)
include(CTest)
include(GoogleTest)

add_subdirectory(src)

if (BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()
