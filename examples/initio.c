/*
 * Author: Houman Brinjcargorabi <houman.brinjcargorabi@intel.com>
 * Copyright (c) 2016 Intel Corporation.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mraa.h"

int main()
{
    // GPIO-PIN
    mraa_gpio_context gpio = (mraa_gpio_context) init_io("gpio-1");
    // GPIO-RAW-RAWPIN
    mraa_gpio_context gpioraw = (mraa_gpio_context) init_io("gpio-raw-131");
    // AIO-PIN
    mraa_aio_context aio = (mraa_aio_context) init_io("aio-0");
    // PWM-PIN
    mraa_pwm_context pwm = (mraa_pwm_context) init_io("pwm-6");
    // PWM-RAW-CHIPID-PIN
    mraa_pwm_context pwmraw= (mraa_pwm_context) init_io("pwm-raw-0,1");
    // UART-INDEX: the index is the one represented internally in the uart_dev array
    mraa_uart_context uart = (mraa_uart_context) init_io("uart-1");
    // UART-RAW-PATH
    mraa_uart_context uartraw = (mraa_uart_context) init_io("uart-raw-/dev/ttyS0");
    // SPI-INDEX: same as UART
    mraa_spi_context spi = (mraa_spi_context) init_io("spi-0");
    // SPI-RAW-BUS-CS: USED to open and use /dev/spidev<BUS>.<CS>
    mraa_spi_context spiraw = (mraa_spi_context) init_io("spi-raw-0-1");
    // I2C-INDEX: same as UART
    mraa_i2c_context i2c = (mraa_i2c_context) init_io("i2c-0");
    // I2C-RAW-BUS
    mraa_i2c_context i2craw = (mraa_i2c_context) init_io("i2c-raw-0");


    // FREE STUFF
    mraa_gpio_close(gpio);
    mraa_gpio_close(gpioraw);
    mraa_aio_close(aio);
    mraa_pwm_close(pwm);
    mraa_pwm_close(pwmraw);
    mraa_uart_close(uart);
    mraa_uart_close(uartraw);
    mraa_spi_close(spi);
    mraa_spi_close(spiraw);
    mraa_i2c_close(i2c);
    mraa_i2c_close(i2craw);
}
