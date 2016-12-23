/*
 * Author: Constantin Musca <constantin.musca@intel.com>
 * Copyright (c) 2016 Intel Corporation.
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

#include "spi.h"
#include "mraa_internal.h"

extern BPeripheralManagerClient *client;
extern char **spi_busses;
extern int spi_busses_count;

mraa_spi_context
mraa_spi_init(int bus)
{
    int rc;
    mraa_spi_context dev;

    if (bus > spi_busses_count) {
        return NULL;
    }

    dev = (mraa_spi_context) calloc(1, sizeof(struct _spi));
    if (dev == NULL) {
        return NULL;
    }

    rc = BPeripheralManagerClient_openSpiDevice(client, spi_busses[bus], &dev->bspi);
    if (rc != 0) {
        free(dev);
        return NULL;
    }

    return dev;
}

mraa_spi_context
mraa_spi_init_raw(unsigned int bus, unsigned int cs)
{
    return NULL;
}

mraa_result_t
mraa_spi_mode(mraa_spi_context dev, mraa_spi_mode_t mode)
{
    int rc;

    if (dev->bspi == NULL) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    switch (mode) {
        case MRAA_SPI_MODE0:
            rc = BSpiDevice_setMode(dev->bspi, SPI_MODE0);
            break;
        case MRAA_SPI_MODE1:
            rc = BSpiDevice_setMode(dev->bspi, SPI_MODE1);
            break;
        case MRAA_SPI_MODE2:
            rc = BSpiDevice_setMode(dev->bspi, SPI_MODE2);
            break;
        case MRAA_SPI_MODE3:
            rc = BSpiDevice_setMode(dev->bspi, SPI_MODE3);
            break;
        default:
            rc = BSpiDevice_setMode(dev->bspi, SPI_MODE0);
            break;
    }
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    dev->mode = mode;

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_spi_frequency(mraa_spi_context dev, int hz)
{
    int rc;

    if (dev->bspi == NULL) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    rc = BSpiDevice_setFrequency(dev->bspi, hz);
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    dev->clock = hz;

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_spi_lsbmode(mraa_spi_context dev, mraa_boolean_t lsb)
{
    int rc;

    if (dev->bspi == NULL) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    if (lsb) {
        rc = BSpiDevice_setBitJustification(dev->bspi, SPI_LSB_FIRST);
    } else {
        rc = BSpiDevice_setBitJustification(dev->bspi, SPI_MSB_FIRST);
    }
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    dev->lsb = lsb;
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_spi_bit_per_word(mraa_spi_context dev, unsigned int bits)
{
    if (dev->bspi == NULL) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    if (BSpiDevice_setBitsPerWord(dev->bspi, bits) != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

int
mraa_spi_write(mraa_spi_context dev, uint8_t data)
{
    int rc;
    uint8_t recv = 0;

    if (dev->bspi == NULL) {
        return -1;
    }

    rc = BSpiDevice_transfer(dev->bspi, &data, &recv, 1);
    if (rc != 0) {
        return -1;
    }

    return (int) recv;
}

int
mraa_spi_write_word(mraa_spi_context dev, uint16_t data)
{
    int rc;
    uint16_t recv = 0;

    if (dev->bspi == NULL) {
        return -1;
    }

    rc = BSpiDevice_transfer(dev->bspi, &data, &recv, 2);
    if (rc != 0) {
        return -1;
    }

    return (int) recv;
}

mraa_result_t
mraa_spi_transfer_buf(mraa_spi_context dev, uint8_t* data, uint8_t* rxbuf, int length)
{
    int rc;

    if (dev->bspi == NULL) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    rc = BSpiDevice_transfer(dev->bspi, data, rxbuf, length);
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_spi_transfer_buf_word(mraa_spi_context dev, uint16_t* data, uint16_t* rxbuf, int length)
{
    int rc;

    if (dev->bspi == NULL) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    // IS IT CORRECT ?
    rc = BSpiDevice_transfer(dev->bspi, data, rxbuf, length * 2);
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

uint8_t*
mraa_spi_write_buf(mraa_spi_context dev, uint8_t* data, int length)
{
    uint8_t *recv = malloc(sizeof(uint8_t) * length);

    if (mraa_spi_transfer_buf(dev, data, recv, length) != MRAA_SUCCESS) {
        free(recv);
        return NULL;
    }
    return recv;
}

uint16_t*
mraa_spi_write_buf_word(mraa_spi_context dev, uint16_t* data, int length)
{
    uint16_t *recv = malloc(sizeof(uint16_t) * length);

    if (mraa_spi_transfer_buf_word(dev, data, recv, length) != MRAA_SUCCESS) {
        free(recv);
        return NULL;
    }
    return recv;
}

mraa_result_t
mraa_spi_stop(mraa_spi_context dev)
{
    if (dev->bspi != NULL) {
        BSpiDevice_delete(dev->bspi);
        dev->bspi = NULL;
    }

    free(dev);
    return MRAA_SUCCESS;
}
