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
/**
 * @file
 * @brief Analog input/output
 *
 * AIO is the anlog input & output interface to libmaa. It is used to read or
 * set the voltage applied to an AIO pin.
 *
 * @snippet analogin_a0.c Interesting
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#include "common.h"
#include "gpio.h"

#define ADC_RAW_RESOLUTION_BITS         (12)
#define ADC_SUPPORTED_RESOLUTION_BITS   (10)

/**
 * Opaque pointer definition to the internal struct _aio. This context refers
 * to one single AIO pin on the board.
 */
typedef struct _aio* maa_aio_context;

/**
 * Initialise an Analog input device, connected to the specified pin
 *
 * @param pin Channel number to read ADC inputs
 * @returns aio context or NULL
 */
maa_aio_context maa_aio_init(unsigned int pin);

/**
 * Read the input voltage
 *
 * @param dev The AIO context
 * @returns The current input voltage, normalised to a 16-bit value
 */
uint16_t maa_aio_read(maa_aio_context dev);

/**
 * Close the analog input context, this will free the memory for the context
 *
 * @param dev The AIO context
 * @return Result of operation
 */
maa_result_t maa_aio_close(maa_aio_context dev);

#ifdef __cplusplus
}
#endif
