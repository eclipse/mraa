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

#include "spi.h"

#define MAX_SIZE 64
#define SPI_MAX_LENGTH 4096

/**
 * A strucutre representing the SPI device
 */
struct _spi {
    /*@{*/
    int devfd; /**< File descriptor to SPI Device */
    /*@}*/
};

maa_spi_context
maa_spi_init()
{
    double bus = maa_check_spi();
    if(bus < 0) {
        fprintf(stderr, "Failed. SPI platform Error\n");
        return NULL;
    }
    maa_spi_context dev = (maa_spi_context) malloc(sizeof(struct _spi));
    memset(dev, 0, sizeof(struct _spi));

    char path[MAX_SIZE];
    sprintf(path, "/dev/spidev%.1f", bus);

    dev->devfd = open(path, O_RDWR);
    if (dev->devfd < 0) {
        fprintf(stderr, "Failed opening SPI Device. bus:%s\n", path);
        free(dev);
        return NULL;
    }

    return dev;
}

maa_result_t
maa_spi_mode(maa_spi_context dev, unsigned short mode)
{
    return MAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

maa_result_t
maa_spi_frequency(maa_spi_context dev, int hz)
{
    return MAA_ERROR_FEATURE_NOT_IMPLEMENTED;
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
    msg.speed_hz = 100000;
    msg.bits_per_word = 8;
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
    msg.speed_hz = 100000;
    msg.bits_per_word = 8;
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
    return MAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}
