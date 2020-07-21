/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2015-2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <error.h>
#include <sys/types.h>

#include <json-c/json.h>

#include <pwd.h>

#include <mraa/uart.h>
#include <mraa/gpio.h>
#include <mraa/i2c.h>
#include <mraa/pwm.h>

#include <mraa_internal.h>

#include <imraa_arduino.h>

#define SYSFS_CLASS_GPIO "/sys/class/gpio"
#define MAX_SIZE 64

typedef struct mraa_io_objects_t {
    const char* type;
    int index;
    bool raw;
    const char* label;
    const char* user;
} mraa_io_objects_t;

void
imraa_handle_IO(struct json_object* jobj)
{
    struct mraa_io_objects_t* mraa_io_obj;
    struct json_object* ioarray;
    int ionum = 0;
    int i;
    if (json_object_object_get_ex(jobj, "IO", &ioarray) == true) {
        ionum = json_object_array_length(ioarray);
        printf("Length of IO array is %d\n", ionum);
        mraa_io_obj = (mraa_io_objects_t*) calloc(ionum, sizeof(mraa_io_objects_t));
        if (!mraa_io_obj) {
            printf("imraa_handle_IO malloc failed\n");
            exit(1);
        }
        int index2 = -1;
        // optional index for io configuration;
        if (json_object_is_type(ioarray, json_type_array)) {
            for (i = 0; i < ionum; i++) {
                struct json_object* ioobj = json_object_array_get_idx(ioarray, i);
                struct json_object* x;
                if (json_object_object_get_ex(ioobj, "type", &x) == true) {
                    mraa_io_obj[i].type = json_object_get_string(x);
                }
                if (json_object_object_get_ex(ioobj, "index", &x) == true) {
                    mraa_io_obj[i].index = json_object_get_int(x);
                }
                if (json_object_object_get_ex(ioobj, "raw", &x) == true) {
                    mraa_io_obj[i].raw = json_object_get_boolean(x);
                }
                if (json_object_object_get_ex(ioobj, "label", &x) == true) {
                    mraa_io_obj[i].label = json_object_get_string(x);
                }
                if (json_object_object_get_ex(ioobj, "index2", &x) == true) {
                    index2 = json_object_get_int(x);
                }
                if (json_object_object_get_ex(ioobj, "user", &x) == true) {
                    mraa_io_obj[i].user = json_object_get_string(x);
                }
                // starting io configuration
                if (strcmp(mraa_io_obj[i].type, "gpio") == 0) {
                    mraa_gpio_context gpio = NULL;
                    if (mraa_io_obj[i].raw) {
                        gpio = mraa_gpio_init_raw(mraa_io_obj[i].index);
                    } else {
                        gpio = mraa_gpio_init(mraa_io_obj[i].index);
                    }
                    if (gpio != NULL) {
                        if (mraa_io_obj[i].user != NULL) {
                            int sysfs_gpio_pin = mraa_gpio_get_pin_raw(gpio);
                            if (sysfs_gpio_pin >= 0) {
                                char bu[MAX_SIZE];
                                snprintf(bu, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/value", sysfs_gpio_pin);
                                struct passwd* user = getpwnam(mraa_io_obj[i].user);
                                if (user != NULL) {
                                    if (chown(bu, user->pw_uid, user->pw_gid) == -1) {
                                        fprintf(stderr, "Couldn't change ownership of file %s to %d:%d\n", bu, user->pw_uid, user->pw_gid);
                                    } else {
                                        fprintf(stderr, "Invalid user %s\n", mraa_io_obj[i].user);
                                    }
                                }
                            }
                            mraa_result_t r = mraa_gpio_owner(gpio, 0);
                            if (r != MRAA_SUCCESS) {
                                mraa_result_print(r);
                            }
                        }
                        mraa_gpio_close(gpio);
                    } else if (strcmp(mraa_io_obj[i].type, "i2c") == 0) {
                        mraa_i2c_context i2c = NULL;
                        if (mraa_io_obj[i].raw) {
                            i2c = mraa_i2c_init_raw(mraa_io_obj[i].index);
                        } else {
                            i2c = mraa_i2c_init(mraa_io_obj[i].index);
                        }
                        if (i2c != NULL) {
                            mraa_i2c_stop(i2c);
                        }
                    } else if (strcmp(mraa_io_obj[i].type, "pwm") == 0) {
                        mraa_pwm_context pwm = NULL;
                        if (mraa_io_obj[i].raw) {
                            pwm = mraa_pwm_init_raw(index2, mraa_io_obj[i].index);
                        } else {
                            pwm = mraa_pwm_init(mraa_io_obj[i].index);
                        }
                        if (pwm != NULL) {
                            mraa_result_t r = mraa_pwm_owner(pwm, 0);
                            if (r != MRAA_SUCCESS) {
                                mraa_result_print(r);
                            }
                            mraa_pwm_close(pwm);
                        }
                    } else if (strcmp(mraa_io_obj[i].type, "spi") == 0) {
                        mraa_spi_context spi = NULL;
                        if (mraa_io_obj[i].raw) {
                            spi = mraa_spi_init_raw(mraa_io_obj[i].index, index2);
                        } else {
                            spi = mraa_spi_init(mraa_io_obj[i].index);
                        }
                        if (spi != NULL) {
                            mraa_spi_stop(spi);
                        }
                    } else if (strcmp(mraa_io_obj[i].type, "uart") == 0) {
                        mraa_uart_context uart = NULL;
                        if (mraa_io_obj[i].raw) {
                            uart = mraa_uart_init_raw(mraa_io_obj[i].label);
                        } else {
                            uart = mraa_uart_init(mraa_io_obj[i].index);
                        }
                        if (uart != NULL) {
                            mraa_uart_stop(uart);
                        }
                    }
                }
            }
        } else {
            fprintf(stderr, "IO array incorrectly parsed\n");
        }
        free(mraa_io_obj);
    }
}
