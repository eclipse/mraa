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
#include "maa_internal.h"

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <sys/stat.h>

#define SYSFS_CLASS_GPIO "/sys/class/gpio"
#define MAX_SIZE 64
#define POLL_TIMEOUT

/**
 * A structure representing a gpio pin.
 */

struct _gpio {
    /*@{*/
    int pin; /**< the pin number, as known to the os. */
    int value_fp; /**< the file pointer to the value of the gpio */
    void (* isr)(); /**< the interupt service request */
    pthread_t thread_id; /**< the isr handler thread id */
    int isr_value_fp; /**< the isr file pointer on the value */
    maa_boolean_t owner; /**< If this context originally exported the pin */
    /*@}*/
};

static maa_result_t
maa_gpio_get_valfp(maa_gpio_context dev)
{
    char bu[MAX_SIZE];
    sprintf(bu, SYSFS_CLASS_GPIO "/gpio%d/value", dev->pin);
    dev->value_fp = open(bu, O_RDWR);
    if (dev->value_fp == -1) {
        return MAA_ERROR_INVALID_RESOURCE;
    }

    return MAA_SUCCESS;
}

maa_gpio_context
maa_gpio_init(int pin)
{
    int pinm = maa_setup_gpio(pin);
    if (pinm < 0)
        return NULL;

    return maa_gpio_init_raw(pinm);
}

maa_gpio_context
maa_gpio_init_raw(int pin)
{
    if (pin < 0)
        return NULL;

    char bu[MAX_SIZE];
    int length;

    maa_gpio_context dev = (maa_gpio_context) malloc(sizeof(struct _gpio));
    memset(dev, 0, sizeof(struct _gpio));
    dev->value_fp = -1;
    dev->isr_value_fp = -1;
    dev->pin = pin;

    char directory[MAX_SIZE];
    snprintf(directory, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/", dev->pin);
    struct stat dir;
    if (stat(directory, &dir) == 0 && S_ISDIR(dir.st_mode)) {
        fprintf(stderr, "GPIO Pin already exporting, continuing.\n");
        dev->owner = 0; // Not Owner
    } else {
        int export = open(SYSFS_CLASS_GPIO "/export", O_WRONLY);
        if (export == -1) {
            fprintf(stderr, "Failed to open export for writing!\n");
            return NULL;
        }
        length = snprintf(bu, sizeof(bu), "%d", dev->pin);
        if (write(export, bu, length*sizeof(char)) == -1) {
            fprintf(stderr, "Failed to write to export\n");
            close(export);
            return NULL;
        }
        dev->owner = 1;
        close(export);
    }
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

    // Wait for it forever or until pthread_cancel
    // poll is a cancelable point like sleep()
    int x = poll (&pfd, 1, -1);

    // do a final read to clear interupt
    read (fd, &c, 1);

    return MAA_SUCCESS;
}

static void*
maa_gpio_interrupt_handler(void* arg)
{
    maa_gpio_context dev = (maa_gpio_context) arg;
    maa_result_t ret;

    // open gpio value with open(3)
    char bu[MAX_SIZE];
    sprintf(bu, SYSFS_CLASS_GPIO "/gpio%d/value", dev->pin);
    dev->isr_value_fp = open(bu, O_RDONLY);

    for (;;) {
        ret = maa_gpio_wait_interrupt(dev->isr_value_fp);
        if (ret == MAA_SUCCESS) {
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
#ifdef SWIGPYTHON
            // In order to call a python object (all python functions are objects) we
            // need to aquire the GIL (Global Interpreter Lock). This may not always be
            // nessecary but especially if doing IO (like print()) python will segfault
            // if we do not hold a lock on the GIL
            PyGILState_STATE gilstate = PyGILState_Ensure();

            PyEval_CallObject((PyObject*)dev->isr, NULL);

            PyGILState_Release (gilstate);
#else
            dev->isr();
#endif
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        } else {
        // we must have got an error code so die nicely
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
            close(dev->isr_value_fp);
            dev->isr_value_fp = -1;
        return NULL;
        }
    }
}

maa_result_t
maa_gpio_edge_mode(maa_gpio_context dev, gpio_edge_t mode)
{
    if (dev->value_fp != -1) {
         close(dev->value_fp);
         dev->value_fp = -1;
    }

    char filepath[MAX_SIZE];
    snprintf(filepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/edge", dev->pin);

    int edge = open(filepath, O_RDWR);
    if (edge == -1) {
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
            close(edge);
            return MAA_ERROR_FEATURE_NOT_IMPLEMENTED;
    }
    if (write(edge, bu, length*sizeof(char)) == -1) {
        fprintf(stderr, "Failed to write to edge\n");
        close(edge);
        return MAA_ERROR_INVALID_RESOURCE;
    }

    close(edge);
    return MAA_SUCCESS;
}

maa_result_t
maa_gpio_isr(maa_gpio_context dev, gpio_edge_t mode, void (*fptr)(void))
{
    // we only allow one isr per maa_gpio_context
    if (dev->thread_id != 0) {
        return MAA_ERROR_NO_RESOURCES;
    }

    maa_gpio_edge_mode(dev, mode);
    dev->isr = fptr;
    pthread_create (&dev->thread_id, NULL, maa_gpio_interrupt_handler, (void *) dev);

    return MAA_SUCCESS;
}

maa_result_t
maa_gpio_isr_exit(maa_gpio_context dev)
{
    maa_result_t ret = MAA_SUCCESS;

    // wasting our time, there is no isr to exit from
    if (dev->thread_id == 0 && dev->isr_value_fp == -1) {
        return ret;
    }

    // stop isr being useful
    ret = maa_gpio_edge_mode(dev, MAA_GPIO_EDGE_NONE);

    if ((dev->thread_id != 0) &&
        (pthread_cancel(dev->thread_id) != 0)) {
        ret = MAA_ERROR_INVALID_HANDLE;
    }

    // close the filehandle in case it's still open
    if (dev->isr_value_fp != -1) {
          if (close(dev->isr_value_fp) != 0) {
              ret = MAA_ERROR_INVALID_PARAMETER;
          }
    }

#ifdef SWIGPYTHON
    // Dereference our Python call back function
    Py_DECREF(dev->isr);
#endif

    // assume our thread will exit either way we just lost it's handle
    dev->thread_id = 0;
    dev->isr_value_fp = -1;

    return ret;
}

maa_result_t
maa_gpio_mode(maa_gpio_context dev, gpio_mode_t mode)
{
    if (dev->value_fp != -1) {
         close(dev->value_fp);
         dev->value_fp = -1;
    }

    char filepath[MAX_SIZE];
    snprintf(filepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/drive", dev->pin);

    int drive = open(filepath, O_WRONLY);
    if (drive == -1) {
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
            close(drive);
            return MAA_ERROR_FEATURE_NOT_IMPLEMENTED;
    }
    if (write(drive, bu, length*sizeof(char)) == -1) {
        fprintf(stderr, "Failed to write to drive mode!\n");
        close(drive);
        return MAA_ERROR_INVALID_RESOURCE;

    }

    close(drive);
    return MAA_SUCCESS;
}

maa_result_t
maa_gpio_dir(maa_gpio_context dev, gpio_dir_t dir)
{
    if (dev == NULL) {
        return MAA_ERROR_INVALID_HANDLE;
    }
    if (dev->value_fp != -1) {
         close(dev->value_fp);
         dev->value_fp = -1;
    }
    char filepath[MAX_SIZE];
    snprintf(filepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/direction", dev->pin);

    int direction = open(filepath, O_RDWR);

    if (direction == -1) {
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
            close(direction);
            return MAA_ERROR_FEATURE_NOT_IMPLEMENTED;
    }

    if (write(direction, bu, length*sizeof(char)) == -1) {
        fprintf(stderr, "Failed to write to direction\n");
        close(direction);
        return MAA_ERROR_INVALID_RESOURCE;
    }

    close(direction);
    return MAA_SUCCESS;
}

int
maa_gpio_read(maa_gpio_context dev)
{
    if (dev->value_fp == -1) {
        if (maa_gpio_get_valfp(dev) != MAA_SUCCESS) {
             fprintf(stderr, "Failed to get value file pointer\n");
        }
    }
    else {
        // if value_fp is new this is pointless
        lseek(dev->value_fp, 0, SEEK_SET);
    }
    char bu[2];
    if (read(dev->value_fp, bu, 2*sizeof(char)) != 2) {
        fprintf(stderr, "Failed to read a sensible value from sysfs");
    }
    lseek(dev->value_fp, 0, SEEK_SET);
    int ret = strtol(bu, NULL, 10);

    return ret;
}

maa_result_t
maa_gpio_write(maa_gpio_context dev, int value)
{
    if (dev->value_fp == -1) {
        maa_gpio_get_valfp(dev);
    }
    if (lseek(dev->value_fp, 0, SEEK_SET) == -1) {
        return MAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length = snprintf(bu, sizeof(bu), "%d", value);
    if (write(dev->value_fp, bu, length*sizeof(char)) == -1) {
        return MAA_ERROR_INVALID_HANDLE;
    }

    return MAA_SUCCESS;
}

static maa_result_t
maa_gpio_unexport_force(maa_gpio_context dev)
{
    int unexport = open(SYSFS_CLASS_GPIO "/unexport", O_WRONLY);
    if (unexport == -1) {
        fprintf(stderr, "Failed to open unexport for writing!\n");
        return MAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length = snprintf(bu, sizeof(bu), "%d", dev->pin);
    if (write(unexport, bu, length*sizeof(char)) == -1) {
        fprintf(stderr, "Failed to write to unexport\n");
        close(unexport);
        return MAA_ERROR_INVALID_RESOURCE;
    }

    close(unexport);
    maa_gpio_isr_exit(dev);
    return MAA_SUCCESS;
}
static maa_result_t
maa_gpio_unexport(maa_gpio_context dev)
{
    if(dev->owner) {
        return maa_gpio_unexport_force(dev);
    }
    return MAA_ERROR_INVALID_RESOURCE;
}

maa_result_t
maa_gpio_close(maa_gpio_context dev)
{
    if (dev->value_fp != -1) {
        close(dev->value_fp);
    }
    maa_gpio_unexport(dev);
    free(dev);
    return MAA_SUCCESS;
}

maa_result_t
maa_gpio_owner(maa_gpio_context dev, maa_boolean_t own)
{
    if (dev == NULL)
        return MAA_ERROR_INVALID_RESOURCE;
    dev->owner = own;
    return MAA_SUCCESS;
}

