/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2018 Intel Corporation.
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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

typedef struct {
    int chip_fd;
    struct gpiochip_info chip_info;
} mraa_gpiod_chip_info;

typedef struct gpioline_info mraa_gpiod_line_info;

void _mraa_free_gpio_groups(mraa_gpio_context dev);
int _mraa_gpiod_ioctl(int fd, unsigned long gpio_request, void* data);

mraa_gpiod_chip_info* mraa_get_chip_info_by_path(const char* path);
mraa_gpiod_chip_info* mraa_get_chip_info_by_name(const char* name);
mraa_gpiod_chip_info* mraa_get_chip_info_by_label(const char* label);
mraa_gpiod_chip_info* mraa_get_chip_info_by_number(unsigned number);

mraa_gpiod_line_info* mraa_get_line_info_by_chip_number(unsigned chip_number, unsigned line_number);
mraa_gpiod_line_info* mraa_get_line_info_by_chip_name(const char* chip_name, unsigned line_number);
mraa_gpiod_line_info* mraa_get_line_info_by_chip_label(const char* chip_label, unsigned line_number);

int mraa_get_lines_handle(int chip_fd, unsigned line_offsets[], unsigned num_lines, unsigned flags, unsigned default_value);
int mraa_set_line_values(int line_handle, unsigned int num_lines, unsigned char input_values[]);
int mraa_get_line_values(int line_handle, unsigned int num_lines, unsigned char output_values[]);
int mraa_get_number_of_gpio_chips();


/* Multiple gpio support. */
typedef struct _gpio_group* mraa_gpiod_group_t;


#ifdef __cplusplus
}
#endif
