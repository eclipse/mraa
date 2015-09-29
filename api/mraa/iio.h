/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2015 Intel Corporation.
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

/**
 * @file
 * @brief iio
 *
 * An iio context represents an IIO device
 *
 * @snippet iio_driver.c Interesting
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>

#include "common.h"

/**
 * Opaque pointer definition to the internal struct _iio
 */
typedef struct _iio* mraa_iio_context;

/**
 * Initialise iio context
 *
 * @param bus iio device to use
 * @return i2c context or NULL
 */
mraa_iio_context mraa_iio_init(int device);

/**
 */
int mraa_iio_get_channel_count(mraa_iio_context dev);

int mraa_iio_get_attr_count(mraa_iio_context dev, int channel);

const char* mraa_iio_get_device_name(mraa_iio_context dev);

/**
 */
mraa_result_t mraa_iio_read(mraa_iio_context dev, const char* attribute, float* data);

/**
 *
 */
mraa_result_t mraa_iio_write(mraa_iio_context dev, const char* attribute);

/**
 * De-inits an mraa_iio_context device
 *
 * @param dev The iio context
 * @return Result of operation
 */
mraa_result_t mraa_iio_stop(mraa_iio_context dev);

#ifdef __cplusplus
}
#endif
