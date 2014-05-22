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
#include "maa_internal.h"

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
    maa_boolean_t lsb; /**< least significant bit mode */
    unsigned int bpw; /**< Bits per word */
    /*@}*/
};

maa_spi_context
maa_spi_init(int bus)
{
    maa_spi_bus_t *spi = maa_setup_spi(bus);
    if(bus < 0) {
        fprintf(stderr, "Failed. SPI platform Error\n");
        return NULL;
    }
    maa_spi_context dev = (maa_spi_context) malloc(sizeof(struct _spi));
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

maa_result_t
maa_spi_mode(maa_spi_context dev, unsigned short mode)
{
    dev->mode = mode;
    return MAA_SUCCESS;
}

maa_result_t
maa_spi_frequency(maa_spi_context dev, int hz)
{
    dev->clock = hz;
    return MAA_SUCCESS;
}

maa_result_t
maa_spi_lsbmode(maa_spi_context dev, maa_boolean_t lsb)
{
    uint8_t lsb_mode = 0;
    if (lsb == 1) {
        lsb_mode = 1;
    }
    if (ioctl (dev->devfd, SPI_IOC_WR_LSB_FIRST, &lsb_mode) < 0) {
        fprintf(stderr, "Failed to set bit order\n");
        return MAA_ERROR_INVALID_RESOURCE;
    }
    dev->lsb = lsb;
    return MAA_SUCCESS;
}

maa_result_t
maa_spi_bit_per_word(maa_spi_context dev, unsigned int bits)
{
    dev->bpw = bits;
    return MAA_SUCCESS;
}

uint8_t
maa_spi_write(maa_spi_context dev, uint8_t data)
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
maa_spi_write_buf(maa_spi_context dev, uint8_t* data, int length)
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

maa_result_t
maa_spi_stop(maa_spi_context dev)
{
    close(dev->devfd);
    return MAA_SUCCESS;
}
