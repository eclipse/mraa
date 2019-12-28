# - Try to find LIBFT4222
# Once done this will define
#
#  LIBFT4222_FOUND - system has LIBFT4222
#  LIBFT4222_INCLUDE_DIRS - the LIBFT4222 include directory
#  LIBFT4222_LIBRARIES - Link these to use LIBFT4222
#  LIBFT4222_DEFINITIONS - Compiler switches required for using LIBFT4222
#
#  Adapted from cmake-modules Google Code project
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#
#  (Changes for LIBFTD2XX) Copyright (c) 2008 Kyle Machulis <kyle@nonpolynomial.com>
#  (Changes for LIBFT4222) Henry Bruce <henry.bruce@intel.com> Copyright (c) 2015 Intel Corporation. 
#
# Redistribution and use is allowed according to the terms of the New BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# SPDX-License-Identifier: BSD-3-Clause

if (LIBFT4222_LIBRARIES AND LIBFT4222_INCLUDE_DIRS)
  # in cache already
  set(LIBFT4222_FOUND TRUE)
else (LIBFT4222_LIBRARIES AND LIBFT4222_INCLUDE_DIRS)
  find_path(LIBFT4222_INCLUDE_DIR
    NAMES
    libft4222.h
    PATHS
    /usr/include
    /usr/local/include
    /opt/local/include
    /sw/include
    )

  SET(FTD4222_LIBNAME ft4222)
  IF(WIN32)
    SET(FTD4222_LIBNAME LibFT4222.lib)
  ENDIF(WIN32)

  find_library(LIBFT4222_LIBRARY
    NAMES
    ${FTD4222_LIBNAME}
    PATHS
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    /sw/lib
    )

  if(LIBFT4222_INCLUDE_DIR)
    set(LIBFT4222_INCLUDE_DIRS
      ${LIBFT4222_INCLUDE_DIR}
      )
  endif(LIBFT4222_INCLUDE_DIR)
  set(LIBFT4222_LIBRARIES
    ${LIBFT4222_LIBRARY}
    )

  if (LIBFT4222_INCLUDE_DIRS AND LIBFT4222_LIBRARIES)
    set(LIBFT4222_FOUND TRUE)
  endif (LIBFT4222_INCLUDE_DIRS AND LIBFT4222_LIBRARIES)

  if (LIBFT4222_FOUND)
    if (NOT LIBFT4222_FIND_QUIETLY)
      message(STATUS "Found LIBFT4222: ${LIBFT4222_LIBRARIES}")
    endif (NOT LIBFT4222_FIND_QUIETLY)
  else (LIBFT4222_FOUND)
    if (LIBFT4222_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find LIBFT4222")
    endif (LIBFT4222_FIND_REQUIRED)
  endif (LIBFT4222_FOUND)

  find_package_handle_standard_args(Ftd4222
      REQUIRED_VARS LIBFT4222_INCLUDE_DIRS LIBFT4222_LIBRARIES)

  # show the LIBFT4222_INCLUDE_DIRS and LIBFT4222_LIBRARIES variables only in the advanced view
  mark_as_advanced(LIBFT4222_INCLUDE_DIRS LIBFT4222_LIBRARIES)

endif (LIBFT4222_LIBRARIES AND LIBFT4222_INCLUDE_DIRS)

