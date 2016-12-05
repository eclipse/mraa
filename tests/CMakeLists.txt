if (BUILDSWIGJAVA)
    add_test (NAME check_clean COMMAND ${PYTHON_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/check_clean.py
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

    add_test (NAME check_samplenames COMMAND ${PYTHON_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/check_samplenames.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

if (BUILDSWIGPYTHON)
    if (PYTHON2INTERP_FOUND)
        set (PYTHON_DEFAULT_PYTHONPATH "${CMAKE_BINARY_DIR}/src/python/python2")
    elseif (PYTHON3INTERP_FOUND)
        set (PYTHON_DEFAULT_PYTHONPATH "${CMAKE_BINARY_DIR}/src/python/python3")
    endif ()
    if (MOCKPLAT)
        add_subdirectory (mock)
    else()
        add_test (NAME py_general COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/general_checks.py)
        set_tests_properties(py_general PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHON_DEFAULT_PYTHONPATH}")

        add_test (NAME py_platform COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/platform_checks.py)
        set_tests_properties(py_platform PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHON_DEFAULT_PYTHONPATH}")

        add_test (NAME py_gpio COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/gpio_checks.py)
        set_tests_properties(py_gpio PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHON_DEFAULT_PYTHONPATH}")
    endif()
endif()
