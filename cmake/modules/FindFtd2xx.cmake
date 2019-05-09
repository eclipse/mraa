# - Try to find libftd2xx
# Once done this will define
#
#  LIBFTD2XX_FOUND - system has libftd2xx
#  LIBFTD2XX_INCLUDE_DIRS - the libftd2xx include directory
#  LIBFTD2XX_LIBRARIES - Link these to use libftd2xx
#  LIBFTD2XX_DEFINITIONS - Compiler switches required for using libftd2xx
#
#  Adapted from cmake-modules Google Code project
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#
#  (Changes for libftd2xx) Copyright (c) 2008 Kyle Machulis <kyle@nonpolynomial.com>
#
# Redistribution and use is allowed according to the terms of the New BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# SPDX-License-Identifier: BSD-3-Clause

if (LIBFTD2XX_LIBRARIES AND LIBFTD2XX_INCLUDE_DIRS)
  # in cache already
  set(LIBFTD2XX_FOUND TRUE)
else (LIBFTD2XX_LIBRARIES AND LIBFTD2XX_INCLUDE_DIRS)
  find_path(LIBFTD2XX_INCLUDE_DIR
    NAMES
    ftd2xx.h
    PATHS
    /usr/include
    /usr/local/include
    /opt/local/include
    /sw/include
    )

  SET(FTD2XX_LIBNAME ftd2xx)
  IF(WIN32)
    SET(FTD2XX_LIBNAME ftd2xx.lib)
  ENDIF(WIN32)

  find_library(LIBFTD2XX_LIBRARY
    NAMES
    ${FTD2XX_LIBNAME}
    PATHS
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    /sw/lib
    )

  if(LIBFTD2XX_INCLUDE_DIR)
    set(LIBFTD2XX_INCLUDE_DIRS
      ${LIBFTD2XX_INCLUDE_DIR}
      )
  endif(LIBFTD2XX_INCLUDE_DIR)
  set(LIBFTD2XX_LIBRARIES
    ${LIBFTD2XX_LIBRARY}
    )

  if (LIBFTD2XX_INCLUDE_DIRS AND LIBFTD2XX_LIBRARIES)
    set(LIBFTD2XX_FOUND TRUE)
  endif (LIBFTD2XX_INCLUDE_DIRS AND LIBFTD2XX_LIBRARIES)

  if (LIBFTD2XX_FOUND)
    if (NOT libftd2xx_FIND_QUIETLY)
      message(STATUS "Found libftd2xx: ${LIBFTD2XX_LIBRARIES}")
    endif (NOT libftd2xx_FIND_QUIETLY)
  else (LIBFTD2XX_FOUND)
    if (libftd2xx_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find libftd2xx")
    endif (libftd2xx_FIND_REQUIRED)
  endif (LIBFTD2XX_FOUND)

  find_package_handle_standard_args(Ftd2xx
      REQUIRED_VARS LIBFTD2XX_INCLUDE_DIRS)

  # show the LIBFTD2XX_INCLUDE_DIRS and LIBFTD2XX_LIBRARIES variables only in the advanced view
  mark_as_advanced(LIBFTD2XX_INCLUDE_DIRS LIBFTD2XX_LIBRARIES)

endif (LIBFTD2XX_LIBRARIES AND LIBFTD2XX_INCLUDE_DIRS)
