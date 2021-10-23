/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include "gpio/gpio_chardev.h"
#include "linux/gpio.h"
#include "mraa_internal.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define SYSFS_CLASS_GPIO "/sys/class/gpio"
#define MAX_SIZE 64
#define POLL_TIMEOUT

#define DEV_DIR "/dev/"
#define CHIP_DEV_PREFIX "gpiochip"
#define STR_SIZE 64

void
_mraa_free_gpio_groups(mraa_gpio_context dev)
{
    mraa_gpiod_group_t gpio_iter;

    for_each_gpio_group(gpio_iter, dev)
    {
        if (gpio_iter->gpio_lines) {
            free(gpio_iter->gpio_lines);
        }

        if (gpio_iter->rw_values) {
            free(gpio_iter->rw_values);
        }

        if (gpio_iter->gpio_group_to_pins_table) {
            free(gpio_iter->gpio_group_to_pins_table);
        }

        if (gpio_iter->gpiod_handle != -1) {
            close(gpio_iter->gpiod_handle);
        }

        if (gpio_iter->event_handles != NULL) {
            for (int j = 0; j < gpio_iter->num_gpio_lines; ++j) {
                close(gpio_iter->event_handles[j]);
            }

            free(gpio_iter->event_handles);
        }

        close(gpio_iter->dev_fd);
    }

    if (dev->gpio_group) {
        free(dev->gpio_group);
    }

    /* Also delete the pin to gpio chip mapping. */
    if (dev->pin_to_gpio_table) {
        free(dev->pin_to_gpio_table);
    }

    /* User provided array saved internally. */
    if (dev->provided_pins) {
        free(dev->provided_pins);
    }

    /* Finally, delete event array. */
    if (dev->events) {
        free(dev->events);
    }
}

void
_mraa_close_gpio_event_handles(mraa_gpio_context dev)
{
    mraa_gpiod_group_t gpio_iter;

    for_each_gpio_group(gpio_iter, dev)
    {
        if (gpio_iter->event_handles != NULL) {
            for (int j = 0; j < gpio_iter->num_gpio_lines; ++j) {
                close(gpio_iter->event_handles[j]);
            }

            free(gpio_iter->event_handles);

            /* In the end, _mraa_free_gpio_groups will be called. */
            gpio_iter->event_handles = NULL;
        }
    }
}

void
_mraa_close_gpio_desc(mraa_gpio_context dev)
{
    mraa_gpiod_group_t gpio_iter;

    for_each_gpio_group(gpio_iter, dev)
    {
        if (gpio_iter->gpiod_handle != -1) {
            close(gpio_iter->gpiod_handle);
            gpio_iter->gpiod_handle = -1;
        }
    }
}

int
_mraa_gpiod_ioctl(int fd, unsigned long gpio_request, void* data)
{
    int status;

    status = ioctl(fd, gpio_request, data);
    if (status < 0) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: ioctl() error %s", strerror(errno));
    }

    return status;
}

int
mraa_get_lines_handle(int chip_fd, unsigned line_offsets[], unsigned num_lines, unsigned flags, unsigned default_value)
{
    int status;
    struct gpiohandle_request __gpio_hreq;

    __gpio_hreq.lines = num_lines;
    memcpy(__gpio_hreq.lineoffsets, line_offsets, num_lines * sizeof __gpio_hreq.lineoffsets[0]);

    if (flags & GPIOHANDLE_REQUEST_OUTPUT) {
        memset(__gpio_hreq.default_values, 0, num_lines * sizeof __gpio_hreq.default_values[0]);
    }
    __gpio_hreq.flags = flags;

    status = _mraa_gpiod_ioctl(chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &__gpio_hreq);
    if (status < 0) {
        syslog(LOG_ERR, "gpiod: ioctl() fail");
        return status;
    }

    if (__gpio_hreq.fd <= 0) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: invalid file descriptor");
    }

    return __gpio_hreq.fd;
}

mraa_gpiod_chip_info*
mraa_get_chip_info_by_path(const char* path)
{
    mraa_gpiod_chip_info* cinfo;
    int chip_fd, status;

    if (path == NULL) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: invalid device path");
        return NULL;
    }

    chip_fd = open(path, O_RDWR | O_CLOEXEC);
    if (chip_fd < 0) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: could not open device file %s", path);
        return NULL;
    }

    cinfo = malloc(sizeof *cinfo);
    if (!cinfo) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: malloc() fail");
        close(chip_fd);
        return NULL;
    }
    cinfo->chip_fd = chip_fd;

    status = _mraa_gpiod_ioctl(cinfo->chip_fd, GPIO_GET_CHIPINFO_IOCTL, &cinfo->chip_info);
    if (status < 0) {
        close(cinfo->chip_fd);
        free(cinfo);
        return NULL;
    }

    return cinfo;
}

mraa_gpiod_chip_info*
mraa_get_chip_info_by_name(const char* name)
{
    mraa_gpiod_chip_info* cinfo;
    char* full_path;

    /* TODO: check for string lengths first. */

    full_path = malloc(STR_SIZE * sizeof *full_path);
    if (!full_path) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: malloc() fail");
        return NULL;
    }

    snprintf(full_path, STR_SIZE, "%s%s", DEV_DIR, name);

    cinfo = mraa_get_chip_info_by_path(full_path);
    free(full_path);

    return cinfo;
}

mraa_gpiod_chip_info*
mraa_get_chip_info_by_label(const char* label)
{
    /* TODO */
    return NULL;
}

mraa_gpiod_chip_info*
mraa_get_chip_info_by_number(unsigned number)
{
    mraa_gpiod_chip_info* cinfo;
    char* full_path;

    /* TODO: check for string lengths first. */

    full_path = malloc(STR_SIZE * sizeof *full_path);
    if (!full_path) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: malloc() fail");
        return NULL;
    }

    snprintf(full_path, STR_SIZE, "%s%s%u", DEV_DIR, CHIP_DEV_PREFIX, number);

    cinfo = mraa_get_chip_info_by_path(full_path);
    free(full_path);

    return cinfo;
}

mraa_gpiod_line_info*
mraa_get_line_info_from_descriptor(int chip_fd, unsigned line_number)
{
    int status;
    mraa_gpiod_line_info* linfo;

    linfo = malloc(sizeof *linfo);

    if (!linfo) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: malloc() fail");
        return NULL;
    }

    linfo->line_offset = line_number;
    status = _mraa_gpiod_ioctl(chip_fd, GPIO_GET_LINEINFO_IOCTL, linfo);
    if (status < 0) {
        free(linfo);
        return NULL;
    }

    return linfo;
}

mraa_gpiod_line_info*
mraa_get_line_info_by_chip_number(unsigned chip_number, unsigned line_number)
{
    mraa_gpiod_line_info* linfo;
    mraa_gpiod_chip_info* cinfo;

    cinfo = mraa_get_chip_info_by_number(chip_number);
    if (!cinfo) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: invalid chip number");
        return NULL;
    }

    linfo = mraa_get_line_info_from_descriptor(cinfo->chip_fd, line_number);

    close(cinfo->chip_fd);
    free(cinfo);

    return linfo;
}

mraa_gpiod_line_info*
mraa_get_line_info_by_chip_name(const char* chip_name, unsigned line_number)
{
    mraa_gpiod_line_info* linfo;
    mraa_gpiod_chip_info* cinfo;

    cinfo = mraa_get_chip_info_by_name(chip_name);
    if (!cinfo) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: invalid chip number");
        return NULL;
    }

    linfo = mraa_get_line_info_from_descriptor(cinfo->chip_fd, line_number);

    close(cinfo->chip_fd);
    free(cinfo);

    return linfo;
}

mraa_gpiod_line_info*
mraa_get_line_info_by_chip_label(const char* chip_label, unsigned line_number)
{
    mraa_gpiod_line_info* linfo;
    mraa_gpiod_chip_info* cinfo;

    cinfo = mraa_get_chip_info_by_label(chip_label);
    if (!cinfo) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: invalid chip number");
        return NULL;
    }

    linfo = mraa_get_line_info_from_descriptor(cinfo->chip_fd, line_number);

    close(cinfo->chip_fd);
    free(cinfo);

    return linfo;
}

int
mraa_set_line_values(int line_handle, unsigned int num_lines, unsigned char input_values[])
{
    int status;
    struct gpiohandle_data __hdata;

    memcpy(__hdata.values, input_values, num_lines * sizeof(unsigned char));

    status = _mraa_gpiod_ioctl(line_handle, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &__hdata);
    if (status < 0) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: ioctl() fail");
    }

    return status;
}

int
mraa_get_line_values(int line_handle, unsigned int num_lines, unsigned char output_values[])
{
    int status;
    struct gpiohandle_data __hdata;

    status = _mraa_gpiod_ioctl(line_handle, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &__hdata);
    if (status < 0) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: ioctl() fail");
        return status;
    }

    memcpy(output_values, __hdata.values, num_lines * sizeof(unsigned char));

    return status;
}


mraa_boolean_t
mraa_is_gpio_line_kernel_owned(mraa_gpiod_line_info* linfo)
{
    return (linfo->flags & GPIOLINE_FLAG_KERNEL);
}

mraa_boolean_t
mraa_is_gpio_line_dir_out(mraa_gpiod_line_info* linfo)
{
    return (linfo->flags & GPIOLINE_FLAG_IS_OUT);
}

mraa_boolean_t
mraa_is_gpio_line_active_low(mraa_gpiod_line_info* linfo)
{
    return (linfo->flags & GPIOLINE_FLAG_ACTIVE_LOW);
}

mraa_boolean_t
mraa_is_gpio_line_open_drain(mraa_gpiod_line_info* linfo)
{
    return (linfo->flags & GPIOLINE_FLAG_OPEN_DRAIN);
}

mraa_boolean_t
mraa_is_gpio_line_open_source(mraa_gpiod_line_info* linfo)
{
    return (linfo->flags & GPIOLINE_FLAG_OPEN_SOURCE);
}

static int
dir_filter(const struct dirent* dir)
{
    return !strncmp(dir->d_name, CHIP_DEV_PREFIX, strlen(CHIP_DEV_PREFIX));
}

int
mraa_get_number_of_gpio_chips()
{
    int num_chips;
    struct dirent** dirs;

    num_chips = scandir("/dev", &dirs, dir_filter, alphasort);
    if (num_chips < 0) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: scandir() error");
        return -1;
    }

    return num_chips;
}

int
mraa_get_chip_infos(mraa_gpiod_chip_info*** cinfos)
{
    int num_chips, i;
    struct dirent** dirs;
    mraa_gpiod_chip_info** cinfo;

    num_chips = scandir("/dev", &dirs, dir_filter, alphasort);
    if (num_chips < 0) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: scandir() error");
        return -1;
    }

    cinfo = (mraa_gpiod_chip_info**) calloc(num_chips, sizeof(mraa_gpiod_chip_info*));
    if (!cinfo) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: Failed to allocate memory for chip info");
        return -1;
    }

    /* Get chip info for all gpiochips present in the platform */
    for (i = 0; i < num_chips; i++) {
        cinfo[i] = mraa_get_chip_info_by_name(dirs[i]->d_name);
        if (!cinfo[i]) {
            syslog(LOG_ERR, "[GPIOD_INTERFACE]: invalid chip %s", dirs[i]->d_name);
            return 0;
        }
    }

    *cinfos = cinfo;

    return num_chips;
}
