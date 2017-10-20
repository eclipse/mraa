/*
 * Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 * Copyright (c) 2017 Linaro Limited.
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
 *
 * Inspired from libgpiod (https://github.com/brgl/libgpiod/)
 *
 * TODO: multiple gpio access support, event support
 *
 */

#include "gpio_chardev.h"
#include "mraa_internal.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/gpio.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

mraa_result_t
mraa_gpio_ioctl(int fd, unsigned long req_flag, void* value)
{
    int status;

    status = ioctl(fd, req_flag, value);
    if (status < 0) {
        syslog(LOG_ERR, "gpio: ioctl call failed: %s", strerror(errno));
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

mraa_gpio_char_context
mraa_gpio_get_chipinfo(mraa_gpio_char_context dev)
{
    mraa_result_t res;

    res = mraa_gpio_ioctl(dev->chip_fd, GPIO_GET_CHIPINFO_IOCTL, &dev->cinfo);
    if (res != MRAA_SUCCESS) {
        syslog(LOG_ERR, "gpio%i: failed to get chipinfo: %s", dev->pin, strerror(errno));
        close(dev->chip_fd);
        free(dev);
        return NULL;
    }

    return dev;
}

mraa_gpio_char_context
mraa_gpio_get_lineinfo(mraa_gpio_char_context dev)
{
    mraa_result_t res;

    memset(dev->linfo.name, 0, sizeof(dev->linfo.name));
    memset(dev->linfo.consumer, 0, sizeof(dev->linfo.consumer));
    dev->linfo.line_offset = dev->pin;
    dev->linfo.flags = 0;

    res = mraa_gpio_ioctl(dev->chip_fd, GPIO_GET_LINEINFO_IOCTL, &dev->linfo);
    if (res != MRAA_SUCCESS) {
        syslog(LOG_ERR, "gpio%i: failed to get lineinfo: %s", dev->pin, strerror(errno));
        close(dev->chip_fd);
        free(dev);
        return NULL;
    }

    return dev;
}

mraa_gpio_char_context
mraa_gpio_char_init(unsigned int chip, unsigned int pin)
{
    mraa_board_t* board = plat;
    if (board == NULL) {
        syslog(LOG_ERR, "gpio%i: init: platform not initialised", pin);
        return NULL;
    }

    if (mraa_is_sub_platform_id(pin)) {
        syslog(LOG_NOTICE, "gpio%i: init: Using sub platform", pin);
        board = board->sub_platform;
        if (board == NULL) {
            syslog(LOG_ERR, "gpio%i: init: Sub platform not initialised", pin);
            return NULL;
        }
        pin = mraa_get_sub_platform_index(pin);
    }

    if (pin) { // We may provide NULL for getting the list of pins for a gpiochip
        if (pin < 0 || pin >= board->phy_pin_count) {
            syslog(LOG_ERR, "gpio: init: pin %i beyond platform pin count (%i)", pin, board->phy_pin_count);
            return NULL;
        }
        if (board->pins[pin].capabilities.gpio != 1) {
            syslog(LOG_ERR, "gpio: init: pin %i not capable of gpio", pin);
            return NULL;
        }
    }
    mraa_gpio_char_context dev = (mraa_gpio_char_context) malloc(sizeof(struct _gpio));
    if (dev == NULL) {
        syslog(LOG_CRIT, "gpio%i: init: Failed to allocate memory for context", pin);
        return NULL;
    }

    dev->pin = board->pins[pin].gpio.pinmap; // chardev pin number
    dev->chip = chip;
    dev->value_fp = -1;
    dev->isr_value_fp = -1;
    dev->isr_thread_terminating = 0;
    dev->phy_pin = pin; // board pin number

    char filepath[MAX_BUF_SIZE];
    snprintf(filepath, MAX_BUF_SIZE, CDEV_PREFIX "%d", dev->chip);

    int fd = open(filepath, O_RDWR);
    if (fd == -1) {
        syslog(LOG_ERR, "gpio%i: init: Failed to open gpio chip%i: %s", pin, chip, strerror(errno));
        free(dev);
        return NULL;
    }

    dev->chip_fd = fd;

    return mraa_gpio_get_chipinfo(dev);
}

mraa_result_t
mraa_gpio_char_set_output(mraa_gpio_char_context dev, mraa_gpio_char_state_t state, mraa_gpio_char_mode_t mode, mraa_gpio_char_value_t val)
{
    mraa_result_t res;

    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: set_output: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    dev->req.flags |= GPIOHANDLE_REQUEST_OUTPUT;

    if (state == MRAA_GPIO_STATE_ACTIVE_LOW)
        dev->req.flags |= GPIOHANDLE_REQUEST_ACTIVE_LOW;

    switch (mode) {
        case MRAA_GPIO_OUTPUT_OPEN_DRAIN:
            dev->req.flags |= GPIOHANDLE_REQUEST_OPEN_DRAIN;
            break;
        case MRAA_GPIO_OUTPUT_OPEN_SOURCE:
            dev->req.flags |= GPIOHANDLE_REQUEST_OPEN_SOURCE;
            break;
        default:
            syslog(LOG_ERR, "gpio%i: set_output: Invalid mode specified: %s", dev->pin, strerror(errno));
            return MRAA_ERROR_INVALID_RESOURCE;
    }

    dev->req.default_values[0] = val;
    dev->req.lineoffsets[0] = dev->pin;
    dev->req.lines = 1;

    res = mraa_gpio_ioctl(dev->chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &dev->req);
    if (res != MRAA_SUCCESS) {
        syslog(LOG_ERR, "gpio%i: set_output: failed to set gpio output: %s", dev->pin, strerror(errno));
        close(dev->chip_fd);
        free(dev);
        return MRAA_ERROR_UNSPECIFIED;
    }

    dev = mraa_gpio_get_lineinfo(dev);
    if (dev == NULL)
        return MRAA_ERROR_UNSPECIFIED;

    return res;
}

mraa_result_t
mraa_gpio_char_set_input(mraa_gpio_char_context dev, mraa_gpio_char_state_t state)
{
    mraa_result_t res;

    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: set_input: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    dev->req.flags |= GPIOHANDLE_REQUEST_INPUT;

    if (state == MRAA_GPIO_STATE_ACTIVE_LOW)
        dev->req.flags |= GPIOHANDLE_REQUEST_ACTIVE_LOW;

    dev->req.lineoffsets[0] = dev->pin;
    dev->req.lines = 1;

    res = mraa_gpio_ioctl(dev->chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &dev->req);
    if (res != MRAA_SUCCESS) {
        syslog(LOG_ERR, "gpio%i: set_input: failed to set gpio input: %s", dev->pin, strerror(errno));
        close(dev->chip_fd);
        free(dev);
        return MRAA_ERROR_UNSPECIFIED;
    }

    dev = mraa_gpio_get_lineinfo(dev);
    if (dev == NULL)
        return MRAA_ERROR_UNSPECIFIED;

    return res;
}

int
mraa_gpio_char_read(mraa_gpio_char_context dev)
{
    mraa_result_t res;

    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: read: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    memset(&dev->data, 0, sizeof(dev->data));

    res = mraa_gpio_ioctl(dev->req.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &dev->data);
    if (res != MRAA_SUCCESS) {
        syslog(LOG_ERR, "gpio%i: read: failed to read gpio: %s", dev->pin, strerror(errno));
        close(dev->chip_fd);
        free(dev);
        return MRAA_ERROR_UNSPECIFIED;
    }

    return dev->data.values[0];
}

mraa_result_t
mraa_gpio_char_write(mraa_gpio_char_context dev, int val)
{
    mraa_result_t res;

    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: write: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    memset(&dev->data, 0, sizeof(dev->data));
    dev->data.values[0] = !!val;

    res = mraa_gpio_ioctl(dev->req.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &dev->data);
    if (res != MRAA_SUCCESS) {
        syslog(LOG_ERR, "gpio%i: write: failed to write gpio: %s", dev->pin, strerror(errno));
        close(dev->chip_fd);
        free(dev);
        return MRAA_ERROR_UNSPECIFIED;
    }

    return res;
}

mraa_result_t
mraa_gpio_char_read_dir(mraa_gpio_char_context dev, mraa_gpio_char_dir_t* dir)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: read_dir: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (dir == NULL) {
        syslog(LOG_ERR, "gpio: read_dir: output parameter for dir is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    *dir = dev->linfo.flags & GPIOLINE_FLAG_IS_OUT ? MRAA_GPIO_OUTPUT : MRAA_GPIO_INPUT;

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_char_read_state(mraa_gpio_char_context dev, mraa_gpio_char_state_t* state)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: read_state: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (state == NULL) {
        syslog(LOG_ERR, "gpio: read_state: output parameter for state is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    *state = dev->linfo.flags & GPIOLINE_FLAG_ACTIVE_LOW ? MRAA_GPIO_STATE_ACTIVE_LOW : MRAA_GPIO_STATE_ACTIVE_HIGH;

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_char_close(mraa_gpio_char_context dev)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: close: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    close(dev->req.fd);
    close(dev->chip_fd);
    free(dev);

    return MRAA_SUCCESS;
}

int
mraa_gpio_char_get_pin(mraa_gpio_char_context dev)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: get_pin: context is invalid");
        return -1;
    }
    return dev->phy_pin;
}

int
mraa_gpio_char_get_pin_raw(mraa_gpio_char_context dev)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: get_pin_raw: context is invalid");
        return -1;
    }
    return dev->pin;
}
