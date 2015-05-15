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
#include "mraa_internal.h"

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define SYSFS_CLASS_GPIO "/sys/class/gpio"
#define MAX_SIZE 64
#define POLL_TIMEOUT

static mraa_result_t
mraa_gpio_get_valfp(mraa_gpio_context dev)
{
    char bu[MAX_SIZE];
    sprintf(bu, SYSFS_CLASS_GPIO "/gpio%d/value", dev->pin);
    dev->value_fp = open(bu, O_RDWR);
    if (dev->value_fp == -1) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

mraa_gpio_context
mraa_gpio_init(int pin)
{
    if (plat == NULL) {
        syslog(LOG_ERR, "gpio: platform not initialised");
        return NULL;
    }
    if (pin < 0 || pin > plat->phy_pin_count) {
        syslog(LOG_ERR, "gpio: pin %i beyond platform definition", pin);
        return NULL;
    }
    if (plat->pins[pin].capabilites.gpio != 1) {
        syslog(LOG_ERR, "gpio: pin %i not capable of gpio", pin);
        return NULL;
    }
    if (plat->pins[pin].gpio.mux_total > 0) {
        if (mraa_setup_mux_mapped(plat->pins[pin].gpio) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "gpio: unable to setup muxes");
            return NULL;
        }
    }

    mraa_gpio_context r = mraa_gpio_init_raw(plat->pins[pin].gpio.pinmap);
    if (r == NULL) {
        syslog(LOG_CRIT, "gpio: mraa_gpio_init_raw(%d) returned error", pin);
        return NULL;
    }
    r->phy_pin = pin;

    if (advance_func->gpio_init_post != NULL) {
        mraa_result_t ret = advance_func->gpio_init_post(r);
        if (ret != MRAA_SUCCESS) {
            free(r);
            return NULL;
        }
    }
    return r;
}

mraa_gpio_context
mraa_gpio_init_raw(int pin)
{
    if (advance_func->gpio_init_pre != NULL) {
        if (advance_func->gpio_init_pre(pin) != MRAA_SUCCESS)
            return NULL;
    }

    if (pin < 0)
        return NULL;

    char bu[MAX_SIZE];
    int length;

    mraa_gpio_context dev = (mraa_gpio_context) malloc(sizeof(struct _gpio));
    if (dev == NULL) {
        syslog(LOG_CRIT, "gpio: Failed to allocate memory for context");
        return NULL;
    }

    memset(dev, 0, sizeof(struct _gpio));
    dev->value_fp = -1;
    dev->isr_value_fp = -1;
    dev->pin = pin;
    dev->phy_pin = -1;

    // then check to make sure the pin is exported.
    char directory[MAX_SIZE];
    snprintf(directory, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/", dev->pin);
    struct stat dir;
    if (stat(directory, &dir) == 0 && S_ISDIR(dir.st_mode)) {
        dev->owner = 0; // Not Owner
    } else {
        int export = open(SYSFS_CLASS_GPIO "/export", O_WRONLY);
        if (export == -1) {
            syslog(LOG_ERR, "gpio: Failed to open export for writing");
            free(dev);
            return NULL;
        }
        length = snprintf(bu, sizeof(bu), "%d", dev->pin);
        if (write(export, bu, length * sizeof(char)) == -1) {
            syslog(LOG_ERR, "gpio: Failed to write %d to export", dev->pin);
            close(export);
            free(dev);
            return NULL;
        }
        dev->owner = 1;
        close(export);
    }

    return dev;
}


static mraa_result_t
mraa_gpio_wait_interrupt(int fd)
{
    unsigned char c;
    struct pollfd pfd;

    if (fd < 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    // setup poll on POLLPRI
    pfd.fd = fd;
    pfd.events = POLLPRI;

    // do an initial read to clear interrupt
    lseek(fd, 0, SEEK_SET);
    read(fd, &c, 1);

    // Wait for it forever or until pthread_cancel
    // poll is a cancelable point like sleep()
    int x = poll(&pfd, 1, -1);

    // do a final read to clear interrupt
    read(fd, &c, 1);

    return MRAA_SUCCESS;
}

static void*
mraa_gpio_interrupt_handler(void* arg)
{
    mraa_gpio_context dev = (mraa_gpio_context) arg;
    mraa_result_t ret;

    // open gpio value with open(3)
    char bu[MAX_SIZE];
    sprintf(bu, SYSFS_CLASS_GPIO "/gpio%d/value", dev->pin);
    int fp = open(bu, O_RDONLY);
    if (fp < 0) {
        syslog(LOG_ERR, "gpio: failed to open gpio%d/value", dev->pin);
        return NULL;
    }
    dev->isr_value_fp = fp;

    for (;;) {
        ret = mraa_gpio_wait_interrupt(dev->isr_value_fp);
        if (ret == MRAA_SUCCESS) {
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
#ifdef SWIGPYTHON
            // In order to call a python object (all python functions are objects) we
            // need to aquire the GIL (Global Interpreter Lock). This may not always be
            // nessecary but especially if doing IO (like print()) python will segfault
            // if we do not hold a lock on the GIL
            PyGILState_STATE gilstate = PyGILState_Ensure();
            PyObject* arglist;
            PyObject* ret;
            arglist = Py_BuildValue("(i)", dev->isr_args);
            if (arglist == NULL) {
                syslog(LOG_ERR, "gpio: Py_BuildValue NULL");
            } else {
                ret = PyEval_CallObject((PyObject*) dev->isr, arglist);
                if (ret == NULL) {
                    syslog(LOG_ERR, "gpio: PyEval_CallObject failed");
                } else {
                    Py_DECREF(ret);
                }
                Py_DECREF(arglist);
            }

            PyGILState_Release(gilstate);
#else
            dev->isr(dev->isr_args);
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

mraa_result_t
mraa_gpio_edge_mode(mraa_gpio_context dev, gpio_edge_t mode)
{
    if (dev->value_fp != -1) {
        close(dev->value_fp);
        dev->value_fp = -1;
    }

    char filepath[MAX_SIZE];
    snprintf(filepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/edge", dev->pin);

    int edge = open(filepath, O_RDWR);
    if (edge == -1) {
        syslog(LOG_ERR, "gpio: Failed to open edge for writing");
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length;
    switch (mode) {
        case MRAA_GPIO_EDGE_NONE:
            length = snprintf(bu, sizeof(bu), "none");
            break;
        case MRAA_GPIO_EDGE_BOTH:
            length = snprintf(bu, sizeof(bu), "both");
            break;
        case MRAA_GPIO_EDGE_RISING:
            length = snprintf(bu, sizeof(bu), "rising");
            break;
        case MRAA_GPIO_EDGE_FALLING:
            length = snprintf(bu, sizeof(bu), "falling");
            break;
        default:
            close(edge);
            return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
    }
    if (write(edge, bu, length * sizeof(char)) == -1) {
        syslog(LOG_ERR, "gpio: Failed to write to edge");
        close(edge);
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    close(edge);
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_isr(mraa_gpio_context dev, gpio_edge_t mode, void (*fptr)(void*), void* args)
{
    // we only allow one isr per mraa_gpio_context
    if (dev->thread_id != 0) {
        return MRAA_ERROR_NO_RESOURCES;
    }

    if (MRAA_SUCCESS != mraa_gpio_edge_mode(dev, mode)) {
        return MRAA_ERROR_UNSPECIFIED;
    }

    dev->isr = fptr;
    dev->isr_args = args;
    pthread_create(&dev->thread_id, NULL, mraa_gpio_interrupt_handler, (void*) dev);

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_isr_exit(mraa_gpio_context dev)
{
    mraa_result_t ret = MRAA_SUCCESS;

    // wasting our time, there is no isr to exit from
    if (dev->thread_id == 0 && dev->isr_value_fp == -1) {
        return ret;
    }

    // stop isr being useful
    ret = mraa_gpio_edge_mode(dev, MRAA_GPIO_EDGE_NONE);

    if ((dev->thread_id != 0)) {
        if ((pthread_cancel(dev->thread_id) != 0) || (pthread_join(dev->thread_id, NULL) != 0)) {
            ret = MRAA_ERROR_INVALID_HANDLE;
        }
    }

    // close the filehandle in case it's still open
    if (dev->isr_value_fp != -1) {
        if (close(dev->isr_value_fp) != 0) {
            ret = MRAA_ERROR_INVALID_PARAMETER;
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

mraa_result_t
mraa_gpio_mode(mraa_gpio_context dev, gpio_mode_t mode)
{
    if (advance_func->gpio_mode_replace != NULL)
        return advance_func->gpio_mode_replace(dev, mode);

    if (advance_func->gpio_mode_pre != NULL) {
        mraa_result_t pre_ret = (advance_func->gpio_mode_pre(dev, mode));
        if (pre_ret != MRAA_SUCCESS)
            return pre_ret;
    }

    if (dev->value_fp != -1) {
        close(dev->value_fp);
        dev->value_fp = -1;
    }

    char filepath[MAX_SIZE];
    snprintf(filepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/drive", dev->pin);

    int drive = open(filepath, O_WRONLY);
    if (drive == -1) {
        syslog(LOG_ERR, "gpio: Failed to open drive for writing");
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length;
    switch (mode) {
        case MRAA_GPIO_STRONG:
            length = snprintf(bu, sizeof(bu), "strong");
            break;
        case MRAA_GPIO_PULLUP:
            length = snprintf(bu, sizeof(bu), "pullup");
            break;
        case MRAA_GPIO_PULLDOWN:
            length = snprintf(bu, sizeof(bu), "pulldown");
            break;
        case MRAA_GPIO_HIZ:
            length = snprintf(bu, sizeof(bu), "hiz");
            break;
        default:
            close(drive);
            return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
    }
    if (write(drive, bu, length * sizeof(char)) == -1) {
        syslog(LOG_ERR, "gpio: Failed to write to drive mode");
        close(drive);
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    close(drive);
    if (advance_func->gpio_mode_post != NULL)
        return advance_func->gpio_mode_post(dev, mode);
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_dir(mraa_gpio_context dev, gpio_dir_t dir)
{
    if (advance_func->gpio_dir_replace != NULL) {
        return advance_func->gpio_dir_replace(dev, dir);
    }
    if (advance_func->gpio_dir_pre != NULL) {
        mraa_result_t pre_ret = (advance_func->gpio_dir_pre(dev, dir));
        if (pre_ret != MRAA_SUCCESS) {
            return pre_ret;
        }
    }

    if (dev == NULL) {
        return MRAA_ERROR_INVALID_HANDLE;
    }
    if (dev->value_fp != -1) {
        close(dev->value_fp);
        dev->value_fp = -1;
    }
    char filepath[MAX_SIZE];
    snprintf(filepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/direction", dev->pin);

    int direction = open(filepath, O_RDWR);

    if (direction == -1) {
        // Direction Failed to Open. If HIGH or LOW was passed will try and set
        // If not fail as usual.
        switch (dir) {
            case MRAA_GPIO_OUT_HIGH:
                return mraa_gpio_write(dev, 1);
            case MRAA_GPIO_OUT_LOW:
                return mraa_gpio_write(dev, 0);
            default:
                return MRAA_ERROR_INVALID_RESOURCE;
        }
    }

    char bu[MAX_SIZE];
    int length;
    switch (dir) {
        case MRAA_GPIO_OUT:
            length = snprintf(bu, sizeof(bu), "out");
            break;
        case MRAA_GPIO_IN:
            length = snprintf(bu, sizeof(bu), "in");
            break;
        case MRAA_GPIO_OUT_HIGH:
            length = snprintf(bu, sizeof(bu), "high");
            break;
        case MRAA_GPIO_OUT_LOW:
            length = snprintf(bu, sizeof(bu), "low");
            break;
        default:
            close(direction);
            return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
    }

    if (write(direction, bu, length * sizeof(char)) == -1) {
        close(direction);
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    close(direction);
    if (advance_func->gpio_dir_post != NULL)
        return advance_func->gpio_dir_post(dev, dir);
    return MRAA_SUCCESS;
}

int
mraa_gpio_read(mraa_gpio_context dev)
{
    if (dev == NULL)
        return -1;

    if (dev->mmap_read != NULL)
        return dev->mmap_read(dev);

    if (dev->value_fp == -1) {
        if (mraa_gpio_get_valfp(dev) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "gpio: Failed to get value file pointer");
            return -1;
        }
    } else {
        // if value_fp is new this is pointless
        lseek(dev->value_fp, 0, SEEK_SET);
    }
    char bu[2];
    if (read(dev->value_fp, bu, 2 * sizeof(char)) != 2) {
        syslog(LOG_ERR, "gpio: Failed to read a sensible value from sysfs");
        return -1;
    }
    lseek(dev->value_fp, 0, SEEK_SET);

    return (int) strtol(bu, NULL, 10);
}

mraa_result_t
mraa_gpio_write(mraa_gpio_context dev, int value)
{
    if (dev == NULL)
        return MRAA_ERROR_INVALID_HANDLE;

    if (dev->mmap_write != NULL)
        return dev->mmap_write(dev, value);

    if (advance_func->gpio_write_pre != NULL) {
        mraa_result_t pre_ret = (advance_func->gpio_write_pre(dev, value));
        if (pre_ret != MRAA_SUCCESS)
            return pre_ret;
    }

    if (dev->value_fp == -1) {
        if (mraa_gpio_get_valfp(dev) != MRAA_SUCCESS) {
            return MRAA_ERROR_INVALID_RESOURCE;
        }
    }

    if (lseek(dev->value_fp, 0, SEEK_SET) == -1) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length = snprintf(bu, sizeof(bu), "%d", value);
    if (write(dev->value_fp, bu, length * sizeof(char)) == -1) {
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (advance_func->gpio_write_post != NULL)
        return advance_func->gpio_write_post(dev, value);
    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_gpio_unexport_force(mraa_gpio_context dev)
{
    int unexport = open(SYSFS_CLASS_GPIO "/unexport", O_WRONLY);
    if (unexport == -1) {
        syslog(LOG_ERR, "gpio: Failed to open unexport for writing");
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length = snprintf(bu, sizeof(bu), "%d", dev->pin);
    if (write(unexport, bu, length * sizeof(char)) == -1) {
        syslog(LOG_ERR, "gpio: Failed to write to unexport");
        close(unexport);
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    close(unexport);
    mraa_gpio_isr_exit(dev);
    return MRAA_SUCCESS;
}
static mraa_result_t
mraa_gpio_unexport(mraa_gpio_context dev)
{
    if (dev->owner) {
        return mraa_gpio_unexport_force(dev);
    }
    return MRAA_ERROR_INVALID_RESOURCE;
}

mraa_result_t
mraa_gpio_close(mraa_gpio_context dev)
{
    mraa_result_t result = MRAA_SUCCESS;

    if (advance_func->gpio_close_pre != NULL) {
        result = advance_func->gpio_close_pre(dev);
    }

    if (dev->value_fp != -1) {
        close(dev->value_fp);
    }
    mraa_gpio_unexport(dev);
    free(dev);
    return result;
}

mraa_result_t
mraa_gpio_owner(mraa_gpio_context dev, mraa_boolean_t own)
{
    if (dev == NULL) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }
    syslog(LOG_DEBUG, "gpio: Set owner to %d", (int) own);
    dev->owner = own;
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_use_mmaped(mraa_gpio_context dev, mraa_boolean_t mmap_en)
{
    if (advance_func->gpio_mmap_setup != NULL) {
        return advance_func->gpio_mmap_setup(dev, mmap_en);
    }

    syslog(LOG_ERR, "gpio: mmap not implemented on this platform");
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

int
mraa_gpio_get_pin(mraa_gpio_context dev)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: context is invalid");
        return -1;
    }
    return dev->phy_pin;
}

int
mraa_gpio_get_pin_raw(mraa_gpio_context dev)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: context is invalid");
        return -1;
    }
    return dev->pin;
}
