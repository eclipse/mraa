/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Author: Longwei Su <lsu@ics.com>
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
#include <json-c/json.h>
#include <libudev.h>

#include <argp.h>
#include <error.h>

#include <mraa/uart.h>
#include <mraa/gpio.h>
#include <mraa/i2c.h>
#include <mraa/pwm.h>

#include <mraa_internal.h>
#include <imraa_lock.h>

const char*
imraa_get_101_serialport()
{
    struct udev* udev;
    struct udev_enumerate* enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device* dev;
    const char* ret = NULL;
    udev = udev_new();
    if (!udev) {
        printf("Can't create udev, check libudev\n");
        exit(1);
    }
    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "tty");
    udev_enumerate_add_match_property(enumerate, "ID_VENDOR_ID", "8087");
    udev_enumerate_add_match_property(enumerate, "ID_MODEL_ID", "0ab6");
    // udev_enumerate_add_match_property(enumerate, "ID_SERIAL",
    // "Intel_ARDUINO_101_AE6642SQ55000RS");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);

    udev_list_entry_foreach(dev_list_entry, devices)
    {
        const char* path;
        path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, path);
        ret = udev_device_get_devnode(dev);
    }
    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    if (ret) {
        printf("Ardunio 101 Device Node Path: %s\n", ret);
    } else {
        printf("Can't detect any Ardunio 101 Device on tty\n");
    }
    return ret;
}

mraa_result_t
imraa_reset_arduino(const char* modem)
{
    mraa_uart_context uart;
    uart = mraa_uart_init_raw(modem);
    mraa_uart_set_baudrate(uart, 1200);

    if (uart == NULL) {
        fprintf(stderr, "UART failed to setup\n");
        return MRAA_ERROR_UNSPECIFIED;
    }
    mraa_uart_stop(uart);
    mraa_deinit();
    return MRAA_SUCCESS;
}

int
imraa_flash_101(const char* bin_path, const char* bin_file_name, const char* tty)
{
    if (imraa_reset_arduino(tty) != MRAA_SUCCESS) {
        fprintf(stderr, "Failed to reset arduino on %s for unknown reason, carrying on...\n", tty);
    }

    char* ln = NULL;
    size_t len = 0;
    bool board_found = false;
    const char* dfu_list = "/dfu-util -d ,8087:0ABA -l";
    size_t bin_path_len = strlen(bin_path);

    char* full_dfu_list = (char*) calloc((bin_path_len + strlen(dfu_list) + 1), sizeof(char));
    if (!full_dfu_list) {
        printf("imraa_flash_101 can't allocate string buffer for dfu list\n");
        exit(1);
    }
    strncat(full_dfu_list, bin_path, strlen(bin_path));
    strncat(full_dfu_list, dfu_list, strlen(dfu_list));

    int i;
    // dfu list is still needed, as the time for reset and recognized is varies from platform to
    // platform.
    // once dfu able to query available devices, then it is ready to flash
    for (i = 0; i < 10 && board_found == false; i++) {
        printf("Waiting for device...\n");
        // dfu-util -d,8087:0ABA -l
        FILE* dfu_result = popen(full_dfu_list, "r");
        if (dfu_result == NULL) {
            printf("Failed to run command\n");
            exit(1);
        }

        if (i == 4) {
            printf("Flashing is taking longer than expected\n");
            printf("Try pressing MASTER_RESET button\n");
        }

        while (getline(&ln, &len, dfu_result) != -1) {
            if (strstr(ln, "sensor_core")) {
                board_found = true;
                printf("Device found!\n");
                break;
            }
        }
        sleep(1);
        if (pclose(dfu_result) != 0) {
            printf("Failed to close command\n");
            exit(1);
        }
    }
    free(ln);

    if (board_found == false) {
        printf("ERROR: Device is not responding.\n");
        exit(1);
    }

    const char* dfu_upload = "/dfu-util -d ,8087:0ABA -D ";
    const char* dfu_option = " -v -a 7 -R";
    int buffersize = bin_path_len + strlen(dfu_upload) + strlen(bin_file_name) + strlen(dfu_option) + 1;
    char* full_dfu_upload = calloc(buffersize, sizeof(char));
    if (!full_dfu_upload) {
        printf("imraa_flash_101 can't allocate string buffer for dfu flash\n");
        exit(1);
    }
    strncat(full_dfu_upload, bin_path, strlen(bin_path));
    strncat(full_dfu_upload, dfu_upload, strlen(dfu_upload));
    strncat(full_dfu_upload, bin_file_name, strlen(bin_file_name));
    strncat(full_dfu_upload, dfu_option, strlen(dfu_option));
    printf("flash cmd: %s\n", full_dfu_upload);
    int status = system(full_dfu_upload);
    if (status != 0) {
        printf("ERROR: Upload failed on %s\n", tty);
        exit(1);
    }
    printf("SUCCESS: Sketch will execute in about 5 seconds.\n");
    free(full_dfu_list);
    free(full_dfu_upload);
    sleep(5);
    return 0;
}

void
imraa_handle_subplatform(struct json_object* jobj, bool force_update)
{
    struct json_object* platform;
    int i, ionum;
    const char* dfu_loc = NULL;
    const char* lockfile_loc = NULL;
    const char* flash_loc = NULL;
    const char* usbserial = NULL;

    struct json_object* dfu_location;
    if (json_object_object_get_ex(jobj, "dfu-utils-location", &dfu_location) == true) {
        if (json_object_is_type(dfu_location, json_type_string)) {
            printf("dfu location: %s\n", json_object_get_string(dfu_location));
            dfu_loc = json_object_get_string(dfu_location);
        } else {
            fprintf(stderr, "dfu location string incorrectly parsed\n");
        }
    }

    struct json_object* lockfile_location;
    if (json_object_object_get_ex(jobj, "lockfile-location", &lockfile_location) == true) {
        if (json_object_is_type(lockfile_location, json_type_string)) {
            printf("lock file location: %s\n", json_object_get_string(lockfile_location));
            lockfile_loc = json_object_get_string(lockfile_location);
        } else {
            fprintf(stderr, "lock file string incorrectly parsed\n");
        }
    }

    if (json_object_object_get_ex(jobj, "Platform", &platform) == true) {
        if (json_object_is_type(platform, json_type_array)) {
            ionum = json_object_array_length(platform);
            for (i = 0; i < ionum; i++) {
                struct json_object* ioobj = json_object_array_get_idx(platform, i);
                json_object_object_foreach(ioobj, key, val)
                {
                    if (strcmp(key, "flash") == 0) {
                        flash_loc = json_object_get_string(val);
                    }
                    if (strcmp(key, "usbserial") == 0) {
                        usbserial = json_object_get_string(val);
                    }
                }
            }
        } else {
            fprintf(stderr, "platform string incorrectly parsed\n");
        }
    }
    if (flash_loc == NULL || usbserial == NULL ) {
        printf("conf didn't give image location or usb serial, skip flashing\n");
        return;
    }
    if (!dfu_loc) {
        dfu_loc = "/usr/bin";
        printf("No dfu path found, using default path /usr/bin instead");
    }
    if (!lockfile_loc) {
        lockfile_loc = "/tmp/imraa.lock";
        printf("No lock path found, using default lock file /tmp/imraa.lock instead");
    }

    // got flash? do flash
    if (access(lockfile_loc, F_OK) != -1 && force_update == false) {
        printf("already exist a lock file, skip flashing\n");
        printf("force upgrade? remove the lockfile or run with force\n");
        return;
    } else {
        fprintf(stdout, "Starting to flash board\n");
        if (force_update) {
            fprintf(stdout, "**Caution: force update mode**\n");
        }

        // TODO flash img checksum, and serialport validation?
        const char* detected_serialport = imraa_get_101_serialport();
        if (detected_serialport == NULL) {
            printf("No subplatform detected, check USB connection, skip flashing\n");
            return;
        }
        if (strcmp(usbserial, "auto") != 0 && strcmp(usbserial, detected_serialport) != 0) {
            printf("given serial port didn't match detected serial port, skip flashing\n");
            return;
        }
        if (imraa_flash_101(dfu_loc, flash_loc, detected_serialport) == 0) {
            imraa_write_lockfile(lockfile_loc, detected_serialport);
        } else {
            fprintf(stderr, "invalid flashing paramenters, please check agian\n");
            fprintf(stderr, "DFU Util location: %s\n", dfu_loc);
            fprintf(stderr, "Flash Img location: %s\n", dfu_loc);
            fprintf(stderr, "USB Serial: %s\n", usbserial);
            fprintf(stderr, "Flash failed, push master reset and try again\n");
        }
    }
}
