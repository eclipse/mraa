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
    int pinm = mraa_setup_gpio(pin);
    if (pinm < 0)
        return NULL;

    mraa_gpio_context r = mraa_gpio_init_raw(pinm);
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
    memset(dev, 0, sizeof(struct _gpio));
    dev->value_fp = -1;
    dev->isr_value_fp = -1;
    dev->pin = pin;
    dev->phy_pin = -1;

    char directory[MAX_SIZE];
    snprintf(directory, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/", dev->pin);
    struct stat dir;
    if (stat(directory, &dir) == 0 && S_ISDIR(dir.st_mode)) {
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

static mraa_result_t
mraa_gpio_write_register(mraa_gpio_context dev,int value)
{
    if (advance_func->gpio_mmaped_write_replace != NULL)
        return advance_func->gpio_mmaped_write_replace(dev,value);
    if (advance_func->gpio_mmaped_write_pre != NULL) {
        mraa_result_t pre_ret = (advance_func->gpio_mmaped_write_pre(dev,value));
        if(pre_ret != MRAA_SUCCESS)
            return pre_ret;
    }
    if (value == 1) {
        *((unsigned *)dev->reg) |= (1<<dev->reg_bit_pos);
        return MRAA_SUCCESS;
    }
    *((unsigned *)dev->reg) &= ~(1<<dev->reg_bit_pos);

    if (advance_func->gpio_mmaped_write_post != NULL)
        return advance_func->gpio_mmaped_write_post(dev,value);
    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_gpio_wait_interrupt(int fd)
{
    unsigned char c;
    struct pollfd pfd;

    // setup poll on POLLPRI
    pfd.fd = fd;
    pfd.events = POLLPRI;

    // do an initial read to clear interupt
    read (fd, &c, 1);

    if (fd <= 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    // Wait for it forever or until pthread_cancel
    // poll is a cancelable point like sleep()
    int x = poll (&pfd, 1, -1);

    // do a final read to clear interupt
    read (fd, &c, 1);

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
    dev->isr_value_fp = open(bu, O_RDONLY);

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
            PyObject *arglist;
            PyObject *ret;
            arglist = Py_BuildValue("(i)", dev->isr_args);
            if (arglist == NULL) {
                fprintf(stdout, "Py_BuildValue NULL\n");
            } else {
                ret = PyEval_CallObject((PyObject*)dev->isr, arglist);
                if (ret == NULL) {
                    fprintf(stdout, "PyEval_CallObject failed\n");
                } else {
                    Py_DECREF(ret);
                }
                Py_DECREF(arglist);
            }

            PyGILState_Release (gilstate);
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
        fprintf(stderr, "Failed to open edge for writing!\n");
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length;
    switch(mode) {
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
    if (write(edge, bu, length*sizeof(char)) == -1) {
        fprintf(stderr, "Failed to write to edge\n");
        close(edge);
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    close(edge);
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_isr(mraa_gpio_context dev, gpio_edge_t mode, void (*fptr)(void *), void * args)
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
    pthread_create (&dev->thread_id, NULL, mraa_gpio_interrupt_handler, (void *) dev);

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

    if ((dev->thread_id != 0) &&
        (pthread_cancel(dev->thread_id) != 0)) {
        ret = MRAA_ERROR_INVALID_HANDLE;
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
        return advance_func->gpio_mode_replace(dev,mode);

    if (advance_func->gpio_mode_pre != NULL) {
        mraa_result_t pre_ret = (advance_func->gpio_mode_pre(dev,mode));
        if(pre_ret != MRAA_SUCCESS)
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
        fprintf(stderr, "Failed to open drive for writing!\n");
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length;
    switch(mode) {
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
    if (write(drive, bu, length*sizeof(char)) == -1) {
        fprintf(stderr, "Failed to write to drive mode!\n");
        close(drive);
        return MRAA_ERROR_INVALID_RESOURCE;

    }

    close(drive);
    if (advance_func->gpio_mode_post != NULL)
        return advance_func->gpio_mode_post(dev,mode);
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_dir(mraa_gpio_context dev, gpio_dir_t dir)
{
    if (advance_func->gpio_dir_replace != NULL)
        return advance_func->gpio_dir_replace(dev,dir);
    if (advance_func->gpio_dir_pre != NULL) {
        mraa_result_t pre_ret = (advance_func->gpio_dir_pre(dev,dir));
        if(pre_ret != MRAA_SUCCESS)
            return pre_ret;
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
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length;
    switch(dir) {
        case MRAA_GPIO_OUT:
            length = snprintf(bu, sizeof(bu), "out");
            break;
        case MRAA_GPIO_IN:
            length = snprintf(bu, sizeof(bu), "in");
            break;
        default:
            close(direction);
            return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
    }

    if (write(direction, bu, length*sizeof(char)) == -1) {
        close(direction);
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    close(direction);
    if (advance_func->gpio_dir_post != NULL)
        return advance_func->gpio_dir_post(dev,dir);
    return MRAA_SUCCESS;
}

int
mraa_gpio_read(mraa_gpio_context dev)
{
    if (dev->value_fp == -1) {
        if (mraa_gpio_get_valfp(dev) != MRAA_SUCCESS) {
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
    return 0;
}

mraa_result_t
mraa_gpio_write(mraa_gpio_context dev, int value)
{
    if (dev->mmap == 1)
        return mraa_gpio_write_register(dev,value);

    if (advance_func->gpio_write_pre != NULL) {
        mraa_result_t pre_ret = (advance_func->gpio_write_pre(dev,value));
        if(pre_ret != MRAA_SUCCESS)
            return pre_ret;
    }

    if (dev->value_fp == -1) {
        mraa_gpio_get_valfp(dev);
    }
    if (lseek(dev->value_fp, 0, SEEK_SET) == -1) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length = snprintf(bu, sizeof(bu), "%d", value);
    if (write(dev->value_fp, bu, length*sizeof(char)) == -1) {
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (advance_func->gpio_write_post != NULL)
        return advance_func->gpio_write_post(dev,value);
    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_gpio_unexport_force(mraa_gpio_context dev)
{
    int unexport = open(SYSFS_CLASS_GPIO "/unexport", O_WRONLY);
    if (unexport == -1) {
        fprintf(stderr, "Failed to open unexport for writing!\n");
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length = snprintf(bu, sizeof(bu), "%d", dev->pin);
    if (write(unexport, bu, length*sizeof(char)) == -1) {
        fprintf(stderr, "Failed to write to unexport\n");
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
    if(dev->owner) {
        return mraa_gpio_unexport_force(dev);
    }
    return MRAA_ERROR_INVALID_RESOURCE;
}

mraa_result_t
mraa_gpio_close(mraa_gpio_context dev)
{
    if (dev->value_fp != -1) {
        close(dev->value_fp);
    }
    mraa_gpio_unexport(dev);
    free(dev);
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_owner(mraa_gpio_context dev, mraa_boolean_t own)
{
    if (dev == NULL)
        return MRAA_ERROR_INVALID_RESOURCE;
    dev->owner = own;
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_use_mmaped(mraa_gpio_context dev, mraa_boolean_t mmap_en)
{
    if (dev ==  NULL) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    if (mraa_pin_mode_test(dev->phy_pin, MRAA_PIN_FAST_GPIO) == 0)
        return MRAA_ERROR_NO_RESOURCES;

    mraa_mmap_pin_t *mmp = mraa_setup_mmap_gpio(dev->phy_pin);
    if (mmp == NULL)
        return MRAA_ERROR_INVALID_RESOURCE;

    if (mmap_en == 1) {
        if (dev->mmap == 0) {
            close(dev->value_fp);
            int fd;
            fd = open(mmp->mem_dev, O_RDWR);
            if (fd < 1) {
                fprintf(stderr, "Unable to open memory device\n");
                return MRAA_ERROR_INVALID_RESOURCE;
            }
            dev->reg_sz = mmp->mem_sz;
            dev->reg = mmap(NULL, dev->reg_sz, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
            dev->reg_bit_pos = mmp->bit_pos;
            dev->mmap = 1;
            close(fd);
            return MRAA_SUCCESS;
        }
        return MRAA_ERROR_INVALID_PARAMETER;
    }

    if (mmap_en == 0) {
        if (dev ->mmap == 1) {
            munmap(dev->reg, dev->reg_sz);
            dev->mmap = 0;
        }
        return MRAA_ERROR_INVALID_PARAMETER;
    }

    return MRAA_SUCCESS;
}
