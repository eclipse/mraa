/*
 * Author: Alex Tereschenko <alext.mkrs@gmail.com>
 * Copyright (c) 2016 Alex Tereschenko.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

#define MOCK_SPI_DEFAULT_FREQ 4000000
#define MOCK_SPI_DEFAULT_MODE MRAA_SPI_MODE0
#define MOCK_SPI_DEFAULT_LSBMODE 0
#define MOCK_SPI_DEFAULT_BIT_PER_WORD 8
// This is XORed with each byte/word of the transmitted message to get the received one
#define MOCK_SPI_REPLY_DATA_MODIFIER_BYTE 0xAB
#define MOCK_SPI_REPLY_DATA_MODIFIER_WORD 0xABBA

mraa_result_t
mraa_mock_spi_init_raw_replace(mraa_spi_context dev, unsigned int bus, unsigned int cs);

mraa_result_t
mraa_mock_spi_stop_replace(mraa_spi_context dev);

mraa_result_t
mraa_mock_spi_bit_per_word_replace(mraa_spi_context dev, unsigned int bits);

mraa_result_t
mraa_mock_spi_lsbmode_replace(mraa_spi_context dev, mraa_boolean_t lsb);

mraa_result_t
mraa_mock_spi_mode_replace(mraa_spi_context dev, mraa_spi_mode_t mode);

mraa_result_t
mraa_mock_spi_frequency_replace(mraa_spi_context dev, int hz);

int
mraa_mock_spi_write_replace(mraa_spi_context dev, uint8_t data);

int
mraa_mock_spi_write_word_replace(mraa_spi_context dev, uint16_t data);

mraa_result_t
mraa_mock_spi_transfer_buf_replace(mraa_spi_context dev, uint8_t* data, uint8_t* rxbuf, int length);

mraa_result_t
mraa_mock_spi_transfer_buf_word_replace(mraa_spi_context dev, uint16_t* data, uint16_t* rxbuf, int length);

#ifdef __cplusplus
}
#endif
