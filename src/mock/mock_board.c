/*
 * Author: Alex Tereschenko <alext.mkrs@gmail.com>
 * Copyright (c) 2016 Alex Tereschenko.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "common.h"
#include "mock/mock_board.h"
#include "mock/mock_board_gpio.h"
#include "mock/mock_board_aio.h"
#include "mock/mock_board_i2c.h"
#include "mock/mock_board_spi.h"
#include "mock/mock_board_pwm.h"
#include "mock/mock_board_uart.h"

#define PLATFORM_NAME "MRAA mock platform"
#define UART_DEV_PATH "dummy"
#define PWM_DEV_PATH  "dummy"

mraa_board_t*
mraa_mock_board()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }

    // General board definitions
    b->platform_name = PLATFORM_NAME;
    b->phy_pin_count = MRAA_MOCK_PINCOUNT;
    b->gpio_count = MRAA_MOCK_GPIO_COUNT;
    b->aio_count = MRAA_MOCK_AIO_COUNT;
    b->adc_raw = 12;
    b->adc_supported = 10;

    int i2c_offset = MRAA_MOCK_GPIO_COUNT + MRAA_MOCK_AIO_COUNT;
    b->i2c_bus_count = MRAA_MOCK_I2C_BUS_COUNT;

    for(int index = 0; index < MRAA_MOCK_I2C_BUS_COUNT; index++) {
        b->i2c_bus[index].bus_id = index;
        b->i2c_bus[index].sda = i2c_offset + 2 * index;
        b->i2c_bus[index].scl = i2c_offset + 2 * index + 1;
    }
    b->def_i2c_bus = b->i2c_bus[0].bus_id;

    int spi_offset = i2c_offset + 2 * MRAA_MOCK_I2C_BUS_COUNT;
    b->spi_bus_count = MRAA_MOCK_SPI_BUS_COUNT;
    b->def_spi_bus = 0;

    for(int index = 0; index < MRAA_MOCK_SPI_BUS_COUNT; index++) {
        b->spi_bus[index].bus_id = index;
        b->spi_bus[index].slave_s = 0;
        b->spi_bus[index].cs = spi_offset + 4 * index;
        b->spi_bus[index].mosi = spi_offset + 4 * index + 1;
        b->spi_bus[index].miso = spi_offset + 4 * index + 2;
        b->spi_bus[index].sclk = spi_offset + 4 * index + 3;
    }

    int pwm_offset = spi_offset +  4 * MRAA_MOCK_SPI_BUS_COUNT;
    b->pwm_dev_count = MRAA_MOCK_PWM_DEV_COUNT;
    b->pwm_default_period = 500;
    b->pwm_max_period = 2147483;
    b->pwm_min_period = 1;

    for(int index = 0; index < MRAA_MOCK_PWM_DEV_COUNT; index++) {
        b->pwm_dev[index].index = pwm_offset + index;
        b->pwm_dev[index].device_path = PWM_DEV_PATH;
    }

    int uart_offset = pwm_offset + MRAA_MOCK_PWM_DEV_COUNT;
    b->uart_dev_count = MRAA_MOCK_UART_DEV_COUNT;
    b->def_uart_dev = 0;

    for(int index = 0; index < MRAA_MOCK_UART_DEV_COUNT; index++) {
        b->uart_dev[index].rx = uart_offset + 2 * index;
        b->uart_dev[index].tx = uart_offset + 2 * index + 1;
        b->uart_dev[index].device_path = UART_DEV_PATH;
    }

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * MRAA_MOCK_PINCOUNT);
    if (b->pins == NULL) {
        goto error;
    }

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b->pins);
        goto error;
    }

    // Replace functions
    b->adv_func->gpio_init_internal_replace = &mraa_mock_gpio_init_internal_replace;
    b->adv_func->gpio_close_replace = &mraa_mock_gpio_close_replace;
    b->adv_func->gpio_dir_replace = &mraa_mock_gpio_dir_replace;
    b->adv_func->gpio_read_dir_replace = &mraa_mock_gpio_read_dir_replace;
    b->adv_func->gpio_read_replace = &mraa_mock_gpio_read_replace;
    b->adv_func->gpio_write_replace = &mraa_mock_gpio_write_replace;
    b->adv_func->gpio_edge_mode_replace = &mraa_mock_gpio_edge_mode_replace;
    b->adv_func->gpio_isr_replace = &mraa_mock_gpio_isr_replace;
    b->adv_func->gpio_isr_exit_replace = &mraa_mock_gpio_isr_exit_replace;
    b->adv_func->gpio_mode_replace = &mraa_mock_gpio_mode_replace;
    b->adv_func->aio_init_internal_replace = &mraa_mock_aio_init_internal_replace;
    b->adv_func->aio_close_replace = &mraa_mock_aio_close_replace;
    b->adv_func->aio_read_replace = &mraa_mock_aio_read_replace;
    b->adv_func->i2c_init_bus_replace = &mraa_mock_i2c_init_bus_replace;
    b->adv_func->i2c_stop_replace = &mraa_mock_i2c_stop_replace;
    b->adv_func->i2c_set_frequency_replace = &mraa_mock_i2c_set_frequency_replace;
    b->adv_func->i2c_address_replace = &mraa_mock_i2c_address_replace;
    b->adv_func->i2c_read_replace = &mraa_mock_i2c_read_replace;
    b->adv_func->i2c_read_byte_replace = &mraa_mock_i2c_read_byte_replace;
    b->adv_func->i2c_read_byte_data_replace = &mraa_mock_i2c_read_byte_data_replace;
    b->adv_func->i2c_read_bytes_data_replace = &mraa_mock_i2c_read_bytes_data_replace;
    b->adv_func->i2c_read_word_data_replace = &mraa_mock_i2c_read_word_data_replace;
    b->adv_func->i2c_write_replace = &mraa_mock_i2c_write_replace;
    b->adv_func->i2c_write_byte_replace = &mraa_mock_i2c_write_byte_replace;
    b->adv_func->i2c_write_byte_data_replace = &mraa_mock_i2c_write_byte_data_replace;
    b->adv_func->i2c_write_word_data_replace = &mraa_mock_i2c_write_word_data_replace;
    b->adv_func->spi_init_raw_replace = &mraa_mock_spi_init_raw_replace;
    b->adv_func->spi_stop_replace = &mraa_mock_spi_stop_replace;
    b->adv_func->spi_bit_per_word_replace = &mraa_mock_spi_bit_per_word_replace;
    b->adv_func->spi_lsbmode_replace = &mraa_mock_spi_lsbmode_replace;
    b->adv_func->spi_mode_replace = &mraa_mock_spi_mode_replace;
    b->adv_func->spi_frequency_replace = &mraa_mock_spi_frequency_replace;
    b->adv_func->spi_write_replace = &mraa_mock_spi_write_replace;
    b->adv_func->spi_write_word_replace = &mraa_mock_spi_write_word_replace;
    b->adv_func->spi_transfer_buf_replace = &mraa_mock_spi_transfer_buf_replace;
    b->adv_func->spi_transfer_buf_word_replace = &mraa_mock_spi_transfer_buf_word_replace;
    b->adv_func->pwm_init_raw_replace = &mraa_mock_pwm_init_raw_replace;
    b->adv_func->pwm_period_replace = &mraa_mock_pwm_write_period_replace;
    b->adv_func->pwm_write_replace = &mraa_mock_pwm_write_duty_replace;
    b->adv_func->pwm_read_replace = &mraa_mock_pwm_read_duty_replace;
    b->adv_func->pwm_enable_replace = &mraa_mock_pwm_enable_replace;
    b->adv_func->uart_init_raw_replace = &mraa_mock_uart_init_raw_replace;
    b->adv_func->uart_set_baudrate_replace = &mraa_mock_uart_set_baudrate_replace;
    b->adv_func->uart_flush_replace = &mraa_mock_uart_flush_replace;
    b->adv_func->uart_sendbreak_replace = &mraa_mock_uart_sendbreak_replace;
    b->adv_func->uart_set_flowcontrol_replace = &mraa_mock_uart_set_flowcontrol_replace;
    b->adv_func->uart_set_mode_replace = &mraa_mock_uart_set_mode_replace;
    b->adv_func->uart_set_non_blocking_replace = &mraa_mock_uart_set_non_blocking_replace;
    b->adv_func->uart_set_timeout_replace = &mraa_mock_uart_set_timeout_replace;
    b->adv_func->uart_data_available_replace = &mraa_mock_uart_data_available_replace;
    b->adv_func->uart_write_replace = &mraa_mock_uart_write_replace;
    b->adv_func->uart_read_replace = &mraa_mock_uart_read_replace;

    // Pin definitions
    int pos = 0;

    for(int index = 0; index < MRAA_MOCK_GPIO_COUNT; index++) {
        snprintf(b->pins[pos].name, 8, "GPIO%d", index);
        b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
        b->pins[pos].gpio.pinmap = index;
        b->pins[pos].gpio.mux_total = 0;
        pos++;
    }

    for(int index = 0; index < MRAA_MOCK_AIO_COUNT; index++) {
        snprintf(b->pins[pos].name, 8, "ADC%d", index);
        b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 1, 0 };
        b->pins[pos].aio.pinmap = 0;
        b->pins[pos].aio.mux_total = 0;
        pos++;
    }

    for(int index = 0; index < MRAA_MOCK_I2C_BUS_COUNT; index++) {
        snprintf(b->pins[pos].name, 8, "I2C%dSDA", index);
        b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
        b->pins[pos].i2c.mux_total = 0;
        b->pins[pos].i2c.pinmap = 0;
        pos++;

        snprintf(b->pins[pos].name, 8, "I2C%dSCL", index);
        b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
        b->pins[pos].i2c.mux_total = 0;
        b->pins[pos].i2c.pinmap = 0;
        pos++;
    }

    for(int index = 0; index < MRAA_MOCK_SPI_BUS_COUNT; index++) {
        snprintf(b->pins[pos].name, 8, "SPI%dCS", index);
        b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
        b->pins[pos].spi.mux_total = 0;
        b->pins[pos].spi.pinmap = 0;
        pos++;

        snprintf(b->pins[pos].name, 9, "SPI%dMOSI", index);
        b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
        b->pins[pos].spi.mux_total = 0;
        b->pins[pos].spi.pinmap = 0;
        pos++;

        snprintf(b->pins[pos].name, 9, "SPI%dMISO", index);
        b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
        b->pins[pos].spi.mux_total = 0;
        b->pins[pos].spi.pinmap = 0;
        pos++;

        snprintf(b->pins[pos].name, 9, "SPI%dSCLK", index);
        b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
        b->pins[pos].spi.mux_total = 0;
        b->pins[pos].spi.pinmap = 0;
        pos++;
    }

    for(int index = 0; index < MRAA_MOCK_PWM_DEV_COUNT; index++) {
        snprintf(b->pins[pos].name, 8, "PWM%d", index);
        b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 1, 0, 0, 0, 0, 0 };
        b->pins[pos].pwm.pinmap = index;
        b->pins[pos].pwm.parent_id = 0;
        b->pins[pos].pwm.mux_total = 0;
        pos++;
    }

    for(int index = 0; index < MRAA_MOCK_UART_DEV_COUNT; index++) {
        snprintf(b->pins[pos].name, 8, "UART%dRX", index);
        b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 };
        b->pins[pos].uart.pinmap = 0;
        b->pins[pos].uart.parent_id = 0;
        b->pins[pos].uart.mux_total = 0;
        pos++;

        snprintf(b->pins[pos].name, 8, "UART%dTX", index);
        b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 };
        b->pins[pos].uart.pinmap = 0;
        b->pins[pos].uart.parent_id = 0;
        b->pins[pos].uart.mux_total = 0;
        pos++;
    }

    return b;

error:
    syslog(LOG_CRIT, "MRAA mock: Platform failed to initialise");
    free(b);
    return NULL;
}
