cmake_minimum_required (VERSION 2.8)
project (maa)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

set(maa_VERSION_MAJOR 0)
set(maa_VERSION_MINOR 1)
set(maa_VERSION_PATCH 1)
set(maa_VERSION_STRING ${maa_VERSION_MAJOR}.${maa_VERSION_MINOR}.${maa_VERSION_PATCH})

set (SWIG_EXECUTABLE /usr/bin/swig)
FIND_PACKAGE (SWIG REQUIRED)
INCLUDE (${SWIG_USE_FILE})

SET (CMAKE_SWIG_FLAGS "")

option (test "Build all tests." OFF)

add_subdirectory (src)
add_subdirectory (examples)

if (test)
  enable_testing ()
  add_subdirectory (tests)
endif ()
