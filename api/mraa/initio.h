/*
 * Author: Noel Eck <noel.eck@intel.com>
 * Copyright (c) 2014-2016 Intel Corporation.
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
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once
/**
 * @file
 * @brief I/O initializer
 *
 * initio allows for string initialization of mraa resources.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "aio.h"
#include "gpio.h"
#include "i2c.h"

#if !defined(PERIPHERALMAN)
#include "iio.h"
#endif

#include "pwm.h"
#include "spi.h"
#include "uart.h"
#include "uart_ow.h"

typedef struct _mraa_io_descriptor {
    int n_aio;
    mraa_aio_context* aios;
    int n_gpio;
    mraa_gpio_context* gpios;
    int n_i2c;
    mraa_i2c_context* i2cs;
#if !defined(PERIPHERALMAN)
    int n_iio;
    mraa_iio_context* iios;
#endif
    int n_pwm;
    mraa_pwm_context* pwms;
    int n_spi;
    mraa_spi_context* spis;
    int n_uart;
    mraa_uart_context* uarts;
    int n_uart_ow;
    mraa_uart_ow_context* uart_ows;

    char* leftover_str;
} mraa_io_descriptor;

/**
  * Initialize a structure of MRAA context elements given a description string.
  *
  * @param strdesc of one or more MRAA IO
  *      io:io_ndx[:option_0:option_1:option_2:option_n][,io:io_ndx]
  *
  *      AIO
  *          AIO_KEY:aio pin[:num_bits]
  *
  *          examples:
  *              a:13                # aio 13
  *              a:13:10             # aio 13, 10 bits
  *
  *      GPIO
  *          GPIO_KEY:gpio pin[:dir:value:mode:edge:input:driver]
  *
  *          examples:
  *              g:13:input          # gpio 13, input
  *              g:13:0:output       # gpio 13, value 0, output
  *
  *      I2C
  *          I2C_KEY:i2c bus[:address:mode]
  *
  *          examples:
  *              i:1:std             # i2c bus 1, STD speed (100 KHz)
  *              i:1:16              # i2c bus 1, address 16
  *              i:0x1:0x10          # i2c bus 1, address 16
  *
  *      IIO
  *          IIO_KEY:iio device
  *
  *          examples:
  *              ii:1                 # iio device 1
  *              ii:0x1               # iio device 1
  *
  *      PWM
  *          PWM_KEY:pwm pin
  *
  *          examples:
  *              p:1                 # pwm pin 1
  *              p:0x1               # pwm pin 1
  *
  *      SPI
  *          SPI_KEY:spi bus[:mode:frequency]
  *
  *          examples:
  *              s:1                 # spi bus 1
  *              s:0x1:mode2:400000  # spi bus 1, mode2 (CPOL = 1, CPHA = 0), 400 KHz
  *
  *      UART
  *          UART_KEY:uart ndx[:baud:mode]
  *
  *          examples:
  *              u:1                 # uart bus 1
  *              u:0x1:9600:8N1      # uart bus 1, 9600 baud, 8 bit byte, no parity, 1 stop bit
  *
  *      UART_OW
  *          UART_OW_KEY:uart_ow ndx
  *
  *          examples:
  *              ow:1                 # uart_ow bus 1
  *              ow:0x1               # uart_ow bus 1
  *
  *
  * @param desc Pointer to structure containing number/pointer collections for initialized IO.
  * @return Result of operation
  */
mraa_result_t mraa_io_init(const char* strdesc, mraa_io_descriptor** desc);

/**
  * Free and close resources used by mraa_io_descriptor structure.
  *
  * @param desc mraa_io_descriptor structure
  * @return Result of operation
  */
mraa_result_t mraa_io_close(mraa_io_descriptor* desc);

#ifdef __cplusplus
}
#endif
