# Mock platform tests
add_test (NAME py_general COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/general_checks.py)
add_test (NAME py_platform COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/platform_checks.py)
add_test (NAME py_gpio_basic COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/gpio_checks_basic.py)
add_test (NAME py_gpio_dir COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/gpio_checks_dir.py)
add_test (NAME py_gpio_write_read COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/gpio_checks_write_read.py)
add_test (NAME py_gpio_edge COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/gpio_checks_edge.py)
add_test (NAME py_gpio_isr COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/gpio_checks_isr.py)
add_test (NAME py_gpio_mode COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/gpio_checks_mode.py)

add_test (NAME py_aio COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/aio_checks.py)

set_tests_properties(py_general
                     py_platform
                     py_gpio_basic
                     py_gpio_dir
                     py_gpio_write_read
                     py_gpio_edge
                     py_gpio_isr
                     py_gpio_mode
                     py_aio
                     PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHON_DEFAULT_PYTHONPATH}")
