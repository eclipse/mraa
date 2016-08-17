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

add_test (NAME py_i2c_freq COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_freq.py)
add_test (NAME py_i2c_addr COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_addr.py)
add_test (NAME py_i2c_read COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_read.py)
add_test (NAME py_i2c_write COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_write.py)
add_test (NAME py_i2c_read_byte COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_read_byte.py)
add_test (NAME py_i2c_write_byte COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_write_byte.py)
add_test (NAME py_i2c_read_byte_data COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_read_byte_data.py)
add_test (NAME py_i2c_write_byte_data COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_write_byte_data.py)
add_test (NAME py_i2c_read_bytes_data COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_read_bytes_data.py)
add_test (NAME py_i2c_read_word_data COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_read_word_data.py)
add_test (NAME py_i2c_write_word_data COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_write_word_data.py)

add_test (NAME py_spi_bit_per_word COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/spi_checks_bit_per_word.py)
add_test (NAME py_spi_checks_lsbmode COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/spi_checks_lsbmode.py)
add_test (NAME py_spi_checks_mode COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/spi_checks_mode.py)
add_test (NAME py_spi_checks_freq COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/spi_checks_freq.py)
add_test (NAME py_spi_checks_write_byte COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/spi_checks_write_byte.py)
add_test (NAME py_spi_checks_write_word COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/spi_checks_write_word.py)
add_test (NAME py_spi_checks_write COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/spi_checks_write.py)

set_tests_properties(py_general
                     py_platform
                     py_gpio_basic
                     py_gpio_dir
                     py_gpio_write_read
                     py_gpio_edge
                     py_gpio_isr
                     py_gpio_mode
                     py_aio
                     py_i2c_freq
                     py_i2c_addr
                     py_i2c_read
                     py_i2c_write
                     py_i2c_read_byte
                     py_i2c_write_byte
                     py_i2c_read_byte_data
                     py_i2c_write_byte_data
                     py_i2c_read_bytes_data
                     py_i2c_read_word_data
                     py_i2c_write_word_data
                     py_spi_bit_per_word
                     py_spi_checks_lsbmode
                     py_spi_checks_mode
                     py_spi_checks_freq
                     py_spi_checks_write_byte
                     py_spi_checks_write_word
                     py_spi_checks_write
                     PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHON_DEFAULT_PYTHONPATH}")
