/*
 * Author: Houman Brinjcargorabi <houman.brinjcargorabi@intel.com>
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mraa.hpp"

int
main()
{
    // GPIO-PIN
    mraa::Gpio* gpio = mraa::initIo<mraa::Gpio>("gpio-1");
    // GPIO-RAW-RAWPIN
    mmraa::Gpio* gpioraw = mraa::initIo<mraa::Gpio> initIo("gpio-raw-131");
    // AIO-PIN
    mraa::Aio aio = mraa::initIo<mraa::Aio>("aio-0");
    // PWM-PIN
    mraa::Pwm pwm = mraa::initIo<mraa::Pwm>("pwm-6");
    // PWM-RAW-CHIPID-PIN
    mraa::Pwm pwmraw = mraa::initIo<mraa::Pwm>("pwm-raw-0,1");
    // UART-INDEX: the index is the one represented internally in the uart_dev array
    mraa::Uart uart = mraa::initIo<mraa::Uart>("uart-1");
    // UART-RAW-PATH
    mraa::Uart uartraw = mraa::initIo<mraa::Uart>("uart-raw-/dev/ttyS0");
    // SPI-INDEX: same as UART
    mraa::Spi spi = mraa::initIo<mraa::Spi>("spi-0");
    // SPI-RAW-BUS-CS: USED to open and use /dev/spidev<BUS>.<CS>
    mraa::Spi spiraw = mraa::initIo<mraa::Spi>("spi-raw-0-1");
    // I2C-INDEX: same as UART
    mraa::I2c i2c = mraa::initIo<mraa::I2c>("i2c-0");
    // I2C-RAW-BUS
    mraa::I2c i2craw = mraa::initIo<mraa::I2c>("i2c-raw-0");

    // FREE STUFF
    delete gpio;
    delete gpioraw;
    delete aio;
    delete pwm;
    delete pwmraw;
    delete uart;
    delete uartraw;
    delete spi;
    delete spiraw;
    delete i2c;
    delete i2craw;
}
