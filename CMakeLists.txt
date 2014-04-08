cmake_minimum_required (VERSION 2.6)
project (maa)

set(SWIG_EXECUTABLE /usr/bin/swig)
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

SET(CMAKE_SWIG_FLAGS "")

add_subdirectory (src)
add_subdirectory (api)
add_subdirectory (examples)
