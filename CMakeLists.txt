cmake_minimum_required (VERSION 2.8)
project (maa)

set(SWIG_EXECUTABLE /usr/bin/swig)
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

SET(CMAKE_SWIG_FLAGS "")

option(test "Build all tests." OFF)

add_subdirectory (src)
add_subdirectory (examples)

if (test)
  enable_testing ()
  add_subdirectory (tests)
endif()
