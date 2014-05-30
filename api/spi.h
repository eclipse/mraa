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

/**
 * @file
 * @brief System Packet Interface
 *
 * This file defines the spi interface for libmaa
 *
 * @snippet spi_mcp4261.c Interesting
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>

#include "maa.h"

/**
 * Opaque pointer definition to the internal struct _spi
 */
typedef struct _spi* maa_spi_context;

/**
 * Initialise SPI_context, uses board mapping. Sets the muxes
 *
 * @param bus Bus to use, as listed in platform definition, normally 0
 * @return Spi context or NULL
 */
maa_spi_context maa_spi_init(int bus);

/**
 * Set the SPI device mode. see spidev 0-3.
 *
 * @param dev The Spi context
 * @param mode The SPI mode, See Linux spidev
 * @return Spi context or NULL
 */
maa_result_t maa_spi_mode(maa_spi_context dev,unsigned short mode);

/** Set the SPI device operating clock frequency.
 *
 * @param dev the Spi context
 * @param hz the frequency in hz
 * @return maa_spi_context The returned initialised SPI context
 */
maa_result_t maa_spi_frequency(maa_spi_context dev, int hz);

/** Write Single Byte to the SPI device.
 *
 * @param dev The Spi context
 * @param data Data to send
 * @return Data received on the miso line
 */
uint8_t maa_spi_write(maa_spi_context dev, uint8_t data);

/** Write Buffer of bytes to the SPI device.
 *
 * @param dev The Spi context
 * @param data to send
 * @param length elements within buffer, Max 4096
 * @return Data received on the miso line, same length as passed in
 */
uint8_t* maa_spi_write_buf(maa_spi_context dev, uint8_t* data, int length);

/**
 * Change the SPI lsb mode
 *
 * @param dev The Spi context
 * @param lsb Use least significant bit transmission. 0 for msbi
 * @return Result of operation
 */
maa_result_t maa_spi_lsbmode(maa_spi_context dev, maa_boolean_t lsb);

/**
 * Set bits per mode on transaction, defaults at 8
 *
 * @param dev The Spi context
 * @param bits bits per word
 * @return Result of operation
 */
maa_result_t maa_spi_bit_per_word(maa_spi_context dev, unsigned int bits);

/**
 * De-inits an maa_spi_context device
 *
 * @param dev The Spi context
 * @return Result of operation
 */
maa_result_t maa_spi_stop(maa_spi_context dev);

#ifdef __cplusplus
}
#endif
