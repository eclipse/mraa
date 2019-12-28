#!/bin/usr/env python

# Author: Houman Brinjcargorabi <houman.brinjcargorabi@intel.com>
# Copyright (c) 2016 Intel Corporation.
#
# SPDX-License-Identifier: MIT

import mraa as m

# GPIO-PIN
m.gpioFromDesc("gpio-1");
# GPIO-RAW-RAWPIN
m.gpioFromDesc("gpio-raw-131");
# AIO-PIN
m.aioFromDesc("aio-1");
# PWM-PIN
m.pwmFromDesc("pwm-6");
# PWM-RAW-CHIPID-PIN
m.pwmFromDesc("pwm-raw-0-1")
# UART-INDEX: the index is the one represented internally in the uart_dev array
m.uartFromDesc("uart-0");
# UART-RAW-PATH
m.uartFromDesc("uart-raw-/dev/ttyS0");
# SPI-INDEX: same as UART
m.spiFromDesc("spi-0");
# SPI-RAW-BUS-CS: USED to open and use /dev/spidev<BUS>.<CS>
m.spiFromDesc("spi-raw-0-1");
# I2C-INDEX: same as UART
m.i2cFromDesc("i2c-0");
# I2C-RAW-BUS
m.i2cFromDesc("i2c-raw-0");
