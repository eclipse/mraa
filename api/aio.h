/*
 * Author: Nandkishor Sonar
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
/** @file
 *
 * This file defines the aio (analog in) interface for libmaa
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <unistd.h>

#include "maa.h"
#include "gpio.h"

#define ADC_RAW_RESOLUTION_BITS         (12)
#define ADC_SUPPORTED_RESOLUTION_BITS   (10)

typedef struct {
    unsigned int channel;
    int adc_in_fp;
} maa_aio_context;

/** Initialise an Analog input device, connected to the specified pin
 *
 * @param aio_channel channel number to read ADC inputs
 *
 * @returns pointer to maa_aio_context structure after initialisation of Analog
 * input pin successfully, else returns null.
 */
maa_aio_context* maa_aio_init(unsigned int aio_channel);

/** Read the input voltage, represented as an unsigned short in the range [0x0,
 * 0xFFFF]
 *
 * @param dev -  pointer to maa_aio_context structure  initialised by
 * maa_aio_init()
 *
 * @returns 16-bit unsigned integer representing the current input voltage,
 * normalised to a 16-bit value
 */
unsigned int maa_aio_read_u16(maa_aio_context* dev);

/** Close the analog input context
 * - Will free the memory for the context.
 *
 * @param dev -  pointer to maa_aio_context structure  initialised by
 * maa_aio_init()
 *
 * @return maa_result_t - result type.
 */
maa_result_t maa_aio_close(maa_aio_context* dev);

#ifdef __cplusplus
}
#endif
