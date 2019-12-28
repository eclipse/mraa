# Copyright (c) 2014 Intel Corporation
# SPDX-License-Identifier: MIT

find_program (SPHINX_EXECUTABLE NAMES sphinx-build
    HINTS
    $ENV{SPHINX_DIR}
    PATH_SUFFIXES bin
    DOC "Sphinx documentation generator"
)

include (FindPackageHandleStandardArgs)

find_package_handle_standard_args (Sphinx DEFAULT_MSG
    SPHINX_EXECUTABLE
)

# Get Sphinx Version
if (SPHINX_EXECUTABLE)
    execute_process(COMMAND ${SPHINX_EXECUTABLE} --version
                    OUTPUT_VARIABLE SPHINX_VERSION_STRING
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_STRIP_TRAILING_WHITESPACE)
    if (SPHINX_VERSION_STRING)
        string(REPLACE "Sphinx (sphinx-build) " "" SPHINX_VERSION ${SPHINX_VERSION_STRING})
        message (STATUS "Sphinx version is ${SPHINX_VERSION}")
    endif ()
endif ()

mark_as_advanced (SPHINX_EXECUTABLE)
