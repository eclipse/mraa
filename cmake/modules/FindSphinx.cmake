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
    if (SPHINX_VERSION_STRING MATCHES "^Sphinx")
        string(REPLACE "Sphinx (sphinx-build) " "" SPHINX_VERSION ${SPHINX_VERSION_STRING})
    else ()
        # newer sphinx builds just say sphinx-build
        string(REPLACE "sphinx-build " "" SPHINX_VERSION ${SPHINX_VERSION_STRING})
    endif ()
    message (STATUS "Sphinx version is ${SPHINX_VERSION}")
endif ()

mark_as_advanced (SPHINX_EXECUTABLE)
