# Mock platform tests
add_test (NAME py_general COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/general_checks.py)
add_test (NAME py_platform COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/platform_checks.py)
add_test (NAME py_gpio_basic COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/gpio_checks_basic.py)
add_test (NAME py_gpio_dir COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/gpio_checks_dir.py)
add_test (NAME py_gpio_write_read COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/gpio_checks_write_read.py)
add_test (NAME py_gpio_edge COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/gpio_checks_edge.py)
add_test (NAME py_gpio_isr COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/gpio_checks_isr.py)
add_test (NAME py_gpio_mode COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/gpio_checks_mode.py)

add_test (NAME py_aio COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/aio_checks.py)

add_test (NAME py_i2c_freq COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_freq.py)
add_test (NAME py_i2c_addr COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_addr.py)
add_test (NAME py_i2c_read COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_read.py)
add_test (NAME py_i2c_write COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_write.py)
add_test (NAME py_i2c_read_byte COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_read_byte.py)
add_test (NAME py_i2c_write_byte COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_write_byte.py)
add_test (NAME py_i2c_read_byte_data COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_read_byte_data.py)
add_test (NAME py_i2c_write_byte_data COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_write_byte_data.py)
add_test (NAME py_i2c_read_bytes_data COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_read_bytes_data.py)
add_test (NAME py_i2c_read_word_data COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_read_word_data.py)
add_test (NAME py_i2c_write_word_data COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/i2c_checks_write_word_data.py)

add_test (NAME py_spi_bit_per_word COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/spi_checks_bit_per_word.py)
add_test (NAME py_spi_checks_lsbmode COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/spi_checks_lsbmode.py)
add_test (NAME py_spi_checks_mode COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/spi_checks_mode.py)
add_test (NAME py_spi_checks_freq COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/spi_checks_freq.py)
add_test (NAME py_spi_checks_write_byte COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/spi_checks_write_byte.py)
add_test (NAME py_spi_checks_write_word COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/spi_checks_write_word.py)
add_test (NAME py_spi_checks_write COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/spi_checks_write.py)

add_test (NAME py_uart_checks_set_baudrate COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/uart_checks_set_baudrate.py)
add_test (NAME py_uart_checks_flush COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/uart_checks_flush.py)
add_test (NAME py_uart_checks_set_flowcontrol COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/uart_checks_set_flowcontrol.py)
add_test (NAME py_uart_checks_set_mode COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/uart_checks_set_mode.py)
add_test (NAME py_uart_checks_set_nonblocking COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/uart_checks_set_nonblocking.py)
add_test (NAME py_uart_checks_set_timeout COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/uart_checks_set_timeout.py)
add_test (NAME py_uart_checks_data_available COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/uart_checks_data_available.py)
add_test (NAME py_uart_checks_write COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/uart_checks_write.py)
add_test (NAME py_uart_checks_read COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/uart_checks_read.py)
add_test (NAME py_uart_checks_sendbreak COMMAND ${PYTHON_DEFAULT_INTERP} ${CMAKE_CURRENT_SOURCE_DIR}/uart_checks_sendbreak.py)

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
                     py_uart_checks_set_baudrate
                     py_uart_checks_flush
                     py_uart_checks_set_flowcontrol
                     py_uart_checks_set_mode
                     py_uart_checks_set_nonblocking
                     py_uart_checks_set_timeout
                     py_uart_checks_data_available
                     py_uart_checks_write
                     py_uart_checks_read
                     py_uart_checks_sendbreak
                     PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHON_DEFAULT_PYTHONPATH}")
