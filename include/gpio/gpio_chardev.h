/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"
#include "linux/gpio.h"

typedef struct {
    int chip_fd;
    struct gpiochip_info chip_info;
} mraa_gpiod_chip_info;

typedef struct gpioline_info mraa_gpiod_line_info;

void _mraa_free_gpio_groups(mraa_gpio_context dev);
void _mraa_close_gpio_event_handles(mraa_gpio_context dev);
void _mraa_close_gpio_desc(mraa_gpio_context dev);
int _mraa_gpiod_ioctl(int fd, unsigned long gpio_request, void* data);

mraa_gpiod_chip_info* mraa_get_chip_info_by_path(const char* path);
mraa_gpiod_chip_info* mraa_get_chip_info_by_name(const char* name);
mraa_gpiod_chip_info* mraa_get_chip_info_by_label(const char* label);
mraa_gpiod_chip_info* mraa_get_chip_info_by_number(unsigned number);

mraa_gpiod_line_info* mraa_get_line_info_from_descriptor(int chip_fd, unsigned line_number);
mraa_gpiod_line_info* mraa_get_line_info_by_chip_number(unsigned chip_number, unsigned line_number);
mraa_gpiod_line_info* mraa_get_line_info_by_chip_name(const char* chip_name, unsigned line_number);
mraa_gpiod_line_info* mraa_get_line_info_by_chip_label(const char* chip_label, unsigned line_number);

int mraa_get_lines_handle(int chip_fd, unsigned line_offsets[], unsigned num_lines, unsigned flags, unsigned default_value);
int mraa_set_line_values(int line_handle, unsigned int num_lines, unsigned char input_values[]);
int mraa_get_line_values(int line_handle, unsigned int num_lines, unsigned char output_values[]);

mraa_boolean_t mraa_is_gpio_line_kernel_owned(mraa_gpiod_line_info *linfo);
mraa_boolean_t mraa_is_gpio_line_dir_out(mraa_gpiod_line_info *linfo);
mraa_boolean_t mraa_is_gpio_line_active_low(mraa_gpiod_line_info *linfo);
mraa_boolean_t mraa_is_gpio_line_open_drain(mraa_gpiod_line_info *linfo);
mraa_boolean_t mraa_is_gpio_line_open_source(mraa_gpiod_line_info *linfo);

int mraa_get_number_of_gpio_chips();
int mraa_get_chip_infos(mraa_gpiod_chip_info*** cinfos);

/* Multiple gpio support. */
typedef struct _gpio_group* mraa_gpiod_group_t;


#ifdef __cplusplus
}
#endif
