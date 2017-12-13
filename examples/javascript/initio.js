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

"use strict";

const mraa = require('mraa');

// GPIO-PIN
mraa.gpioFromDesc("gpio-1");
// GPIO-RAW-RAWPIN
mraa.gpioFromDesc("gpio-raw-131");
// AIO-PIN
mraa.aioFromDesc("aio-1");
//PWM-PIN
mraa.pwmFromDesc("pwm-6");
// PWM-RAW-CHIPID-PIN
mraa.pwmFromDesc("pwm-raw-0-1")
// UART-INDEX: the index is the one represented internally in the uart_dev array
mraa.uartFromDesc("uart-0");
// UART-RAW-PATH
mraa.uartFromDesc("uart-raw-/dev/ttyS0");
// SPI-INDEX: same as UART
mraa.spiFromDesc("spi-0");
// SPI-RAW-BUS-CS: USED to open and use /dev/spidev<BUS>.<CS>
mraa.spiFromDesc("spi-raw-0-1");
// I2C-INDEX: same as UART
mraa.i2cFromDesc("i2c-0");
// I2C-RAW-BUS
mraa.i2cFromDesc("i2c-raw-0");
