cmake_minimum_required (VERSION 2.8)
project (maa)

FIND_PACKAGE (Threads)

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

# Appends the cmake/modules path to MAKE_MODULE_PATH variable.
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

# Make a version file containing the current version from git.
include (GetGitRevisionDescription)
git_describe (VERSION "--tags")
if ("x_${VERSION}" STREQUAL "x_GIT-NOTFOUND")
  message (WARNING " - Install git to compile a production libmaa!")
  set (VERSION "v0.2.7-dirty")
endif ()

message (INFO " - MAA Version ${VERSION}")

#parse the version information into pieces.
string (REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION}")
string (REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION}")
string (REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION}")
string (REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+\\-([0-9]+).*" "\\1" VERSION_COMMIT "${VERSION}")
string (REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+-[0-9]+\\-(.*)" "\\1" VERSION_SHA1 "${VERSION}")
set (VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/version.c.in
                ${CMAKE_CURRENT_BINARY_DIR}/src/version.c)

# this is the library version, independant of git revision
set (maa_VERSION_MAJOR ${VERSION_MAJOR})
set (maa_VERSION_MINOR ${VERSION_MINOR})
set (maa_VERSION_PATCH ${VERSION_PATCH})
set (maa_VERSION_STRING ${maa_VERSION_MAJOR}.${maa_VERSION_MINOR}.${maa_VERSION_PATCH})

set (CMAKE_SWIG_FLAGS "")

option (GTEST "Build all gtests." OFF)
option (BUILDDOC "Build all doc." OFF)
option (BUILDSWIG "Build swig modules." ON)
option (BUILDSWIGPYTHON "Build swig python modules." ON)
option (BUILDSWIGNODE "Build swig node modules." ON)
option (IPK "Generate IPK using CPack" OFF)

if (GTEST)
  enable_testing ()
  add_subdirectory (tests)
endif ()

if (BUILDDOC)
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
endif ()

if (IPK)
  include (TargetArch)
  target_architecture (DETECTED_ARCH)
  message( INFO " - Target arch is ${DETECTED_ARCH}")

  set(CPACK_GENERATOR "DEB")
  set(OPKG_ARCH ${DETECTED_ARCH})
  set(CPACK_BINARY_DIR ${CMAKE_BINARY_DIR})
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Intel IoT-Devkit") #required
  set(CPACK_PACKAGE_VERSION ${maa_VERSION_MAJOR}.${maa_VERSION_MINOR}.${maa_VERSION_PATCH}.${VERSION_COMMIT})
  set(CPACK_DEBIAN_PACKAGE_SECTION "libs")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${DETECTED_ARCH})
  set(CPACK_SYSTEM_NAME ${DETECTED_ARCH})
  set(CPACK_DEBIAN_PACKAGE_PROVIDES "maa-devel maa-dbg maa-docs")
  set(WDIR "${CPACK_TOPLEVEL_DIRECTORY}/${CPACK_PACKAGE_FILE_NAME}")
  include (CPack)
endif()

add_subdirectory (src)
add_subdirectory (examples)
