/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
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
#include "gpio.h"

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

#define SYSFS_CLASS_GPIO "/sys/class/gpio"
#define MAX_SIZE 64
#define POLL_TIMEOUT

static maa_result_t
maa_gpio_get_valfp(maa_gpio_context *dev)
{
    char bu[MAX_SIZE];
    sprintf(bu, SYSFS_CLASS_GPIO "/gpio%d/value", dev->pin);

    if ((dev->value_fp = fopen(bu, "r+b")) == NULL) {
        return MAA_ERROR_INVALID_RESOURCE;
    }

    return MAA_SUCCESS;
}

maa_gpio_context*
maa_gpio_init(int pin)
{
    int pinm = maa_check_gpio(pin);
    if (pinm < 0)
        return NULL;

    return maa_gpio_init_raw(pinm);
}

maa_gpio_context*
maa_gpio_init_raw(int pin)
{
    if (pin < 0)
        return NULL;

    FILE *export_f;
    char bu[MAX_SIZE];
    int length;

    maa_gpio_context* dev = (maa_gpio_context*) malloc(sizeof(maa_gpio_context));
    memset(dev, 0, sizeof(maa_gpio_context));
    dev->pin = pin;

    if ((export_f = fopen(SYSFS_CLASS_GPIO "/export", "w")) == NULL) {
        fprintf(stderr, "Failed to open export for writing!\n");
        return NULL;
    }
    length = snprintf(bu, sizeof(bu), "%d", dev->pin);
    fwrite(bu, sizeof(char), length, export_f);

    fclose(export_f);
    return dev;
}

static maa_result_t
maa_gpio_wait_interrupt(int fd)
{
    unsigned char c;
    struct pollfd pfd;

    // setup poll on POLLPRI
    pfd.fd = fd;
    pfd.events = POLLPRI;

    // do an initial read to clear interupt
    read (fd, &c, 1);

    if (fd <= 0) {
        return MAA_ERROR_INVALID_RESOURCE;
    }

    // Wait for it forever
    int x = poll (&pfd, 1, -1);

    // do a final read to clear interupt
    read (fd, &c, 1);

    return MAA_SUCCESS;
}

static void*
maa_gpio_interrupt_handler(void* arg)
{
    maa_gpio_context* dev = (maa_gpio_context*) arg;
    maa_result_t ret;

    // open gpio value with open(3)
    char bu[MAX_SIZE];
    sprintf(bu, SYSFS_CLASS_GPIO "/gpio%d/value", dev->pin);
    dev->isr_value_fp = open(bu, O_RDONLY);

    for (;;) {
        ret = maa_gpio_wait_interrupt(dev->isr_value_fp);
        if (ret == MAA_SUCCESS) {
            dev->isr();
        } else {
	    // we must have got an error code so die nicely
            close(dev->isr_value_fp);
	    return;
        }
    }
}

maa_result_t
maa_gpio_edge_mode(maa_gpio_context *dev, gpio_edge_t mode)
{
    if (dev->value_fp != NULL) {
         dev->value_fp = NULL;
    }

    char filepath[MAX_SIZE];
    snprintf(filepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/edge", dev->pin);

    FILE *edge;
    if ((edge= fopen(filepath, "w")) == NULL) {
        fprintf(stderr, "Failed to open edge for writing!\n");
        return MAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length;
    switch(mode) {
        case MAA_GPIO_EDGE_NONE:
            length = snprintf(bu, sizeof(bu), "none");
            break;
        case MAA_GPIO_EDGE_BOTH:
            length = snprintf(bu, sizeof(bu), "both");
            break;
        case MAA_GPIO_EDGE_RISING:
            length = snprintf(bu, sizeof(bu), "rising");
            break;
        case MAA_GPIO_EDGE_FALLING:
            length = snprintf(bu, sizeof(bu), "falling");
            break;
        default:
            fclose(edge);
            return MAA_ERROR_FEATURE_NOT_IMPLEMENTED;
    }
    fwrite(bu, sizeof(char), length, edge);

    fclose(edge);
    dev->value_fp = NULL;
    return MAA_SUCCESS;
}

maa_result_t
maa_gpio_isr(maa_gpio_context *dev, gpio_edge_t mode, void (*fptr)(void))
{
    maa_gpio_edge_mode(dev, mode);
    dev->isr = fptr;
    pthread_create (&dev->thread_id, NULL, maa_gpio_interrupt_handler, (void *) dev);

    return MAA_SUCCESS;
}

maa_result_t
maa_gpio_isr_exit(maa_gpio_context *dev)
{
    maa_result_t ret = MAA_SUCCESS;

    if (dev->thread_id == 0) {
        return ret;
    }

    if (pthread_kill(dev->thread_id) != 0) {
       ret = MAA_ERROR_INVALID_HANDLE;
    }
    if (close(dev->isr_value_fp) != 0) {
       ret = MAA_ERROR_INVALID_PARAMETER;
    }

    // this is only required if we had an isr setup
    if (ret == MAA_SUCCESS) {
       ret = maa_gpio_edge_mode(dev, MAA_GPIO_EDGE_NONE);
    }

    return ret;
}

maa_result_t
maa_gpio_mode(maa_gpio_context *dev, gpio_mode_t mode)
{
    if (dev->value_fp != NULL) {
         dev->value_fp = NULL;
    }

    char filepath[MAX_SIZE];
    snprintf(filepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/drive", dev->pin);

    FILE *drive;
    if ((drive = fopen(filepath, "w")) == NULL) {
        fprintf(stderr, "Failed to open drive for writing!\n");
        return MAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length;
    switch(mode) {
        case MAA_GPIO_STRONG:
            length = snprintf(bu, sizeof(bu), "strong");
            break;
        case MAA_GPIO_PULLUP:
            length = snprintf(bu, sizeof(bu), "pullup");
            break;
        case MAA_GPIO_PULLDOWN:
            length = snprintf(bu, sizeof(bu), "pulldown");
            break;
        case MAA_GPIO_HIZ:
            length = snprintf(bu, sizeof(bu), "hiz");
            break;
        default:
            fclose(drive);
            return MAA_ERROR_FEATURE_NOT_IMPLEMENTED;
    }
    fwrite(bu, sizeof(char), length, drive);

    fclose(drive);
    dev->value_fp = NULL;
    return MAA_SUCCESS;
}

maa_result_t
maa_gpio_dir(maa_gpio_context *dev, gpio_dir_t dir)
{
    if (dev == NULL)
        return MAA_ERROR_INVALID_HANDLE;
    if (dev->value_fp != NULL) {
         dev->value_fp = NULL;
    }
    char filepath[MAX_SIZE];
    snprintf(filepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/direction", dev->pin);

    FILE *direction;
    if ((direction = fopen(filepath, "w")) == NULL) {
        fprintf(stderr, "Failed to open direction for writing!\n");
        return MAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length;
    switch(dir) {
        case MAA_GPIO_OUT:
            length = snprintf(bu, sizeof(bu), "out");
            break;
        case MAA_GPIO_IN:
            length = snprintf(bu, sizeof(bu), "in");
            break;
        default:
            fclose(direction);
            return MAA_ERROR_FEATURE_NOT_IMPLEMENTED;
    }
    fwrite(bu, sizeof(char), length, direction);

    fclose(direction);
    dev->value_fp = NULL;
    return MAA_SUCCESS;
}

int
maa_gpio_read(maa_gpio_context *dev)
{
    if (dev->value_fp == NULL) {
        maa_gpio_get_valfp(dev);
    }
    fseek(dev->value_fp, SEEK_SET, 0);
    char bu[2];
    fread(bu, 2, 1, dev->value_fp);
    fseek(dev->value_fp, SEEK_SET, 0);
    int ret = strtol(bu, NULL, 10);

    return ret;
}

maa_result_t
maa_gpio_write(maa_gpio_context *dev, int value)
{
    if (dev->value_fp == NULL) {
        maa_gpio_get_valfp(dev);
    }
    if (fseek(dev->value_fp, SEEK_SET, 0) != 0) {
        return MAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length = snprintf(bu, sizeof(bu), "%d", value);
    fwrite(bu, sizeof(char), length, dev->value_fp);

    if (fseek(dev->value_fp, SEEK_SET, 0) != 0) {
        return MAA_ERROR_INVALID_RESOURCE;
    }
    if (ferror(dev->value_fp) != 0)
        return MAA_ERROR_INVALID_RESOURCE;
    return MAA_SUCCESS;
}

maa_result_t
maa_gpio_unexport(maa_gpio_context *dev)
{
    FILE *unexport_f;

    if ((unexport_f = fopen(SYSFS_CLASS_GPIO "/unexport", "w")) == NULL) {
        fprintf(stderr, "Failed to open unexport for writing!\n");
        return MAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length = snprintf(bu, sizeof(bu), "%d", dev->pin);
    fwrite(bu, sizeof(char), length, unexport_f);
    fclose(unexport_f);

    maa_gpio_isr_exit(dev);

    return MAA_SUCCESS;
}

maa_result_t
maa_gpio_close(maa_gpio_context *dev)
{
    if (ferror(dev->value_fp) != 0) {
        return MAA_ERROR_INVALID_RESOURCE;
    }

    maa_gpio_unexport(dev);
    free(dev);
    return MAA_SUCCESS;
}
