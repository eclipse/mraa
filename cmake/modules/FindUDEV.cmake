# Copyright (c) 2016 Intel Corporation
# SPDX-License-Identifier: MIT

# UDEV_FOUND - true if library and headers were found
# UDEV_INCLUDE_DIRS - include directories
# UDEV_LIBRARIES - library directories

find_package (PkgConfig)
pkg_check_modules (PC_UDEV QUIET udev)

find_path (UDEV_INCLUDE_DIR libudev.h HINTS ${PC_UDEV_INCLUDEDIR}
${PC_UDEV_INCLUDE_DIRS} PATH_SUFFIXES udev)

find_library (UDEV_LIBRARY NAMES udev libudev HINTS ${PC_UDEV_LIBDIR}
${PC_UDEV_LIBRARY_DIRS})

set (UDEV_LIBRARIES ${UDEV_LIBRARY})
set (UDEV_INCLUDE_DIRS ${UDEV_INCLUDE_DIR})

include (FindPackageHandleStandardArgs)

find_package_handle_standard_args (UDEV DEFAULT_MSG UDEV_LIBRARY UDEV_INCLUDE_DIR)

mark_as_advanced (UDEV_INCLUDE_DIR UDEV_LIBRARY)
