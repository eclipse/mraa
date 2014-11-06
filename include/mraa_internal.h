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

#ifdef __cplusplus
extern "C" {
#endif

#include <syslog.h>

#include "common.h"
#include "mraa_internal_types.h"
#include "mraa_adv_func.h"

extern mraa_adv_func_t* advance_func;
extern mraa_board_t* plat;

/**
 * Takes in pin information and sets up the multiplexors.
 *
 * @param meta
 * @return mraa result type indicating success of actions.
 */
mraa_result_t mraa_setup_mux_mapped(mraa_pin_t meta);

/**
 * Setup gpio mux to go straight to SoC, galileo.
 *
 * @param pin physical pin to use
 * @return mraa_mmap_pin_t
 */
mraa_mmap_pin_t* mraa_setup_mmap_gpio(int pin);

/**
 * Setup uart muxes to exposes the pins physically.
 *
 * @param index of the uart in the board definition to expose physically
 * @return mraa_result_t of operation
 */
mraa_result_t mraa_setup_uart(int index);

#ifdef __cplusplus
}
#endif
