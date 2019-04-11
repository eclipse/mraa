if (PYTHON2INTERP_FOUND AND BUILDSWIGJAVA)
    add_test (NAME check_clean COMMAND ${PYTHON_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/check_clean.py
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

    add_test (NAME check_samplenames COMMAND ${PYTHON_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/check_samplenames.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif ()

if (BUILDSWIGPYTHON)
    if (PYTHON2INTERP_FOUND AND EXISTS "${CMAKE_BINARY_DIR}/src/python/python2" AND NOT USEPYTHON3TESTS)
        set (PYTHON_DEFAULT_PYTHONPATH "${CMAKE_BINARY_DIR}/src/python/python2")
        set (PYTHON_DEFAULT_INTERP "${PYTHON2_EXECUTABLE}")
    elseif (PYTHON3INTERP_FOUND AND EXISTS "${CMAKE_BINARY_DIR}/src/python/python3")
        set (PYTHON_DEFAULT_PYTHONPATH "${CMAKE_BINARY_DIR}/src/python/python3")
        set (PYTHON_DEFAULT_INTERP "${PYTHON3_EXECUTABLE}")
    elseif (USEPYTHON3TESTS)
        message (FATAL "Failed to find python3{,-dev} but forced python3 tests")
    endif ()

    if (DEFINED PYTHON_DEFAULT_INTERP)
        if (MOCKPLAT)
            add_subdirectory (mock)
        else ()
            add_test (NAME py_general COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/general_checks.py)
        set_tests_properties (py_general PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHON_DEFAULT_PYTHONPATH}")

            add_test (NAME py_platform COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/platform_checks.py)
            set_tests_properties (py_platform PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHON_DEFAULT_PYTHONPATH}")

            add_test (NAME py_gpio COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/gpio_checks.py)
            set_tests_properties (py_gpio PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHON_DEFAULT_PYTHONPATH}")
        endif ()
    else ()
        message (STATUS "Could not run tests since python interpreter or python bindings not built")
    endif ()
endif ()

# Add mraa unit tests
add_subdirectory(unit)
