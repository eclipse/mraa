/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <json-c/json.h>
#include <libudev.h>

#include <argp.h>
#include <error.h>

#include <mraa/uart.h>
#include <mraa/gpio.h>
#include <mraa/i2c.h>
#include <mraa/pwm.h>

#include <mraa_internal.h>

void
imraa_write_lockfile(const char* lock_file_location, const char* serialport)
{
    FILE* fh;
    char str[10];
    json_object* platform1 = json_object_new_object();
    snprintf(str, 10, "%d", MRAA_NULL_PLATFORM);
    json_object_object_add(platform1, "id", json_object_new_string(str));

    json_object* platform2 = json_object_new_object();
    snprintf(str, 10, "%d", MRAA_GENERIC_FIRMATA);
    json_object_object_add(platform2, "id", json_object_new_string(str));
    json_object_object_add(platform2, "uart", json_object_new_string(serialport));

    json_object* platfroms = json_object_new_array();
    json_object_array_add(platfroms, platform1);
    json_object_array_add(platfroms, platform2);
    json_object* lock_file = json_object_new_object();
    json_object_object_add(lock_file, "Platform", platfroms);
    fh = fopen(lock_file_location, "w");
    if (fh != NULL) {
        fputs(json_object_to_json_string_ext(lock_file, JSON_C_TO_STRING_PRETTY), fh);
        fclose(fh);
    } else {
        fprintf(stderr, "can't write to lock file\n");
    }
    json_object_put(lock_file);
}
