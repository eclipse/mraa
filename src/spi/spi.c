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

#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "spi.h"
#include "mraa_internal.h"

#define MAX_SIZE 64
#define SPI_MAX_LENGTH 4096

/**
 * A structure representing the SPI device
 */
struct _spi {
    /*@{*/
    int devfd; /**< File descriptor to SPI Device */
    int mode; /**< Spi mode see spidev.h */
    int clock; /**< clock to run transactions at */
    mraa_boolean_t lsb; /**< least significant bit mode */
    unsigned int bpw; /**< Bits per word */
    /*@}*/
};

mraa_spi_context
mraa_spi_init(int bus)
{
    mraa_spi_bus_t *spi = mraa_setup_spi(bus);
    if(bus < 0) {
        fprintf(stderr, "Failed. SPI platform Error\n");
        return NULL;
    }
    mraa_spi_context dev = (mraa_spi_context) malloc(sizeof(struct _spi));
    memset(dev, 0, sizeof(struct _spi));

    char path[MAX_SIZE];
    sprintf(path, "/dev/spidev%u.%u", spi->bus_id, spi->slave_s);

    dev->devfd = open(path, O_RDWR);
    if (dev->devfd < 0) {
        fprintf(stderr, "Failed opening SPI Device. bus:%s\n", path);
        free(dev);
        return NULL;
    }
    dev->bpw = 8;
    dev->clock = 4000000;
    dev->lsb = 0;
    dev->mode = 0;

    return dev;
}

mraa_result_t
mraa_spi_mode(mraa_spi_context dev, mraa_spi_mode_t mode)
{
    uint8_t spi_mode = 0;
    switch (mode) {
        case MODE0:
            spi_mode = SPI_MODE_0;
            break;
        case MODE1:
            spi_mode = SPI_MODE_1;
            break;
        case MODE2:
            spi_mode = SPI_MODE_2;
            break;
        case MODE3:
            spi_mode = SPI_MODE_3;
            break;
        default:
            spi_mode = SPI_MODE_0;
            break;
    }

    if (ioctl (dev->devfd, SPI_IOC_WR_MODE, &spi_mode) < 0) {
        fprintf(stderr, "Failed to set spi mode\n");
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    dev->mode = spi_mode;
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_spi_frequency(mraa_spi_context dev, int hz)
{
    dev->clock = hz;
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_spi_lsbmode(mraa_spi_context dev, mraa_boolean_t lsb)
{
    uint8_t lsb_mode = 0;
    if (lsb == 1) {
        lsb_mode = 1;
    }
    if (ioctl (dev->devfd, SPI_IOC_WR_LSB_FIRST, &lsb_mode) < 0) {
        fprintf(stderr, "Failed to set bit order\n");
        return MRAA_ERROR_INVALID_RESOURCE;
    }
    dev->lsb = lsb;
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_spi_bit_per_word(mraa_spi_context dev, unsigned int bits)
{
    dev->bpw = bits;
    return MRAA_SUCCESS;
}

uint8_t
mraa_spi_write(mraa_spi_context dev, uint8_t data)
{
    struct spi_ioc_transfer msg;
    memset(&msg, 0, sizeof(msg));

    uint16_t length = 1;

    uint8_t recv = 0;
    msg.tx_buf = (unsigned long) &data;
    msg.rx_buf = (unsigned long) &recv;
    msg.speed_hz = dev->clock;
    msg.bits_per_word = dev->bpw;
    msg.delay_usecs = 0;
    msg.len = length;
    if (ioctl(dev->devfd, SPI_IOC_MESSAGE(1), &msg) < 0) {
        fprintf(stderr, "Failed to perform dev transfer\n");
        return -1;
    }
    return recv;
}

uint8_t*
mraa_spi_write_buf(mraa_spi_context dev, uint8_t* data, int length)
{
    struct spi_ioc_transfer msg;
    memset(&msg, 0, sizeof(msg));

    uint8_t* recv = malloc(sizeof(uint8_t) * length);

    msg.tx_buf = (unsigned long) data;
    msg.rx_buf = (unsigned long) recv;
    msg.speed_hz = dev->clock;
    msg.bits_per_word = dev->bpw;
    msg.delay_usecs = 0;
    msg.len = length;
    if (ioctl(dev->devfd, SPI_IOC_MESSAGE(1), &msg) < 0) {
        fprintf(stderr, "Failed to perform dev transfer\n");
        return NULL;
    }
    return recv;
}

mraa_result_t
mraa_spi_stop(mraa_spi_context dev)
{
    close(dev->devfd);
    free(dev);
    return MRAA_SUCCESS;
}
