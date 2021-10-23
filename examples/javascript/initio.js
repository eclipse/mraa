/*
 * Author: Houman Brinjcargorabi <houman.brinjcargorabi@intel.com>
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
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
