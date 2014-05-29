/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Copyright (c) 2014 Intel Corporation.
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

#pragma once

#include "maa.h"

/** Setup gpio
 *
 * Will check input is valid for gpio and will also setup required multiplexers.
 * @param pin the pin as read from the board surface. i.e IO3 would be 3/
 * @return the pin as found in the pinmap
 */
unsigned int maa_setup_gpio(int pin);

/** Setup Analog interface
 *
 * Will check input is valid for aio and will also setup required multiplexers.
 * @param pin the pin as read from the board surface. i.e A3 would be 3/
 * @return the pin as found in the pinmap
 */
unsigned int maa_setup_aio(int pin);

/** Setup i2c interface, sets up multiplexer on device.
 *
 * @return unsigned int if using /dev/i2c-2 returned would be 2
 */
unsigned int maa_setup_i2c(int bus);

/** Setup spi interface, sets up multiplexer on device.
 *
 * @return spi bus type
 */
maa_spi_bus_t* maa_setup_spi(int bus);

/** Setup PWM
 *
 * Will check input is valid for pwm and will also setup required multiplexers.
 * IF the pin also does gpio (strong chance), DO NOTHING, REV D quirk
 * @param pin the pin as read from the board surface.
 * @return the pwm pin_info_t of that IO pin
 */
maa_pin_t* maa_setup_pwm(int pin);

/** Setup gpio mux to go straight to SoC, galileo.
 *
 * @param pin physical pin to use
 * @return maa_mmap_pin_t
 */
maa_mmap_pin_t* maa_setup_mmap_gpio(int pin);
