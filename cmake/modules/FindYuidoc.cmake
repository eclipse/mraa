# Copyright (c) 2015 Intel Corporation
# SPDX-License-Identifier: MIT

find_program (YUIDOC_EXECUTABLE NAMES yuidoc
  HINTS $ENV{YUIDOC_DIR}
    PATHS usr usr/local
  PATH_SUFFIXES bin
  DOC "Yuidoc documentation generator"
)

include (FindPackageHandleStandardArgs)

find_package_handle_standard_args (Yuidoc DEFAULT_MSG
  YUIDOC_EXECUTABLE
)

# Get Yuidoc version
if (YUIDOC_EXECUTABLE)
  execute_process(COMMAND ${YUIDOC_EXECUTABLE} --version
                  ERROR_VARIABLE YUIDOC_VERSION
                  OUTPUT_STRIP_TRAILING_WHITESPACE
                  ERROR_STRIP_TRAILING_WHITESPACE)
  if (YUIDOC_VERSION)
    message (STATUS "Yuidoc version is ${YUIDOC_VERSION}")
  endif ()
endif ()

mark_as_advanced (YUIDOC_EXECUTABLE)
