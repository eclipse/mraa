cmake_minimum_required (VERSION 2.8)
project (maa)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

set(maa_VERSION_MAJOR 0)
set(maa_VERSION_MINOR 2)
set(maa_VERSION_PATCH 0)
set(maa_VERSION_STRING ${maa_VERSION_MAJOR}.${maa_VERSION_MINOR}.${maa_VERSION_PATCH})

set (SWIG_EXECUTABLE /usr/bin/swig)
find_package (SWIG REQUIRED)
include (${SWIG_USE_FILE})

set (CMAKE_SWIG_FLAGS "")

option (test "Build all tests." OFF)

add_subdirectory (src)
add_subdirectory (examples)

if (test)
  enable_testing ()
  add_subdirectory (tests)
endif ()

# add a target to generate API documentation with Doxygen
find_package (Doxygen)
if (DOXYGEN_FOUND)
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target (doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
  )
endif (DOXYGEN_FOUND)
