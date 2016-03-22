/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Author: Longwei Su <lsu@ics.com>
 * Copyright (c) 2015 Intel Corporation.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <json-c/json.h>
#include <libudev.h>

#include <mraa/uart.h>
#include <mraa/gpio.h>
#include <mraa/i2c.h>
#include <mraa/pwm.h>

#include <mraa_internal.h>

#define IMRAA_CONF_FILE "/etc/imraa.conf"

typedef struct mraa_io_objects_t {
    const char* type;
    int index;
    bool raw;
    const char* label;
} mraa_io_objects_t;

const char*
imraa_list_serialport()
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
        printf("force upgrade? remove the lockfile or run with force\n", lockfile_loc);
        return;
    } else {
        fprintf(stdout, "Starting to flash board\n");
        if (force_update) {
            fprintf(stdout, "**Caution: force update mode**\n");
        }

        // TODO flash img checksum, and serialport validation?
        const char* detected_serialport = imraa_list_serialport();
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
        mraa_io_obj = (mraa_io_objects_t*) malloc(ionum * sizeof(mraa_io_objects_t));
        if (!mraa_io_obj) {
            printf("imraa_handle_IO malloc failed\n");
            exit(1);
        }
        int index2 = -1;
        ; // optional index for io configuration;
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
                // starting io configuration
                if (strcmp(mraa_io_obj[i].type, "gpio") == 0) {
                    mraa_gpio_context gpio = NULL;
                    if (mraa_io_obj[i].raw) {
                        gpio = mraa_gpio_init_raw(mraa_io_obj[i].index);
                    } else {
                        gpio = mraa_gpio_init(mraa_io_obj[i].index);
                    }
                    if (gpio != NULL) {
                        mraa_result_t r = mraa_gpio_owner(gpio, 0);
                        if (r != MRAA_SUCCESS) {
                            mraa_result_print(r);
                        }
                        mraa_gpio_close(gpio);
                    }
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
        } else {
            fprintf(stderr, "IO array incorrectly parsed\n");
        }
        free(mraa_io_obj);
    }
}

int
check_version(struct json_object* jobj)
{
    struct json_object* imraa_version;
    if (json_object_object_get_ex(jobj, "version", &imraa_version) == true) {
        if (json_object_is_type(imraa_version, json_type_string)) {
            printf("imraa version is %s good\n", json_object_get_string(imraa_version));
            // TODO check version?
        } else {
            fprintf(stderr, "version string incorrectly parsed\n");
            return -1;
        }
    } else {
        fprintf(stderr, "no version specified\n");
    }
    return 0;
}

void
print_version()
{
    fprintf(stdout, "Version %s on %s\n", mraa_get_version(), mraa_get_platform_name());
}

void
print_help()
{
    fprintf(stdout, "version           Get mraa version and board name\n");
    fprintf(stdout, "force             Force update subplatform\n");
    fprintf(stdout, "conf_path         Force update with override configuration\n");
}

void
print_command_error()
{
    fprintf(stdout, "Invalid command, options are:\n");
    print_help();
    exit(EXIT_FAILURE);
}

int
main(int argc, char** argv)
{
    char* buffer = NULL;
    char* imraa_conf_file = IMRAA_CONF_FILE;
    long fsize;
    bool force_update = false;
    int i = 0;
    uint32_t ionum = 0;

    if (argc > 2) {
        print_command_error();
    }

    if (argc > 1) {
        if (strcmp(argv[1], "help") == 0) {
            print_help();
            return EXIT_SUCCESS;
        } else if (strcmp(argv[1], "version") == 0) {
            print_version();
            return EXIT_SUCCESS;
        } else if (strcmp(argv[1], "force") == 0) {
            force_update = true;
        } else {
            imraa_conf_file = argv[1];
            force_update = true;
        }
    }

    FILE* fh = fopen(imraa_conf_file, "r");
    if (fh == NULL) {
        fprintf(stderr, "Failed to open configuration file\n");
        return EXIT_FAILURE;
    }

    fseek(fh, 0, SEEK_END);
    fsize = ftell(fh) + 1;
    fseek(fh, 0, SEEK_SET);
    buffer = calloc(fsize, sizeof(char));
    if (buffer != NULL) {
        int result = fread(buffer, sizeof(char), fsize, fh);
        if (result != fsize) {
            printf("imraa conf reading error");
        }
    } else {
        printf("imraa read_conf buffer can't allocated\n");
        exit(1);
    }

    imraa_init();

    json_object* jobj = json_tokener_parse(buffer);
    if (check_version(jobj) != 0) {
        printf("version of configuration file is not compatible, please check again\n");
    } else {
        mraa_platform_t type = mraa_get_platform_type();
        imraa_handle_subplatform(jobj, force_update);
        if (type == MRAA_NULL_PLATFORM || type == MRAA_UNKNOWN_PLATFORM) {
            printf("imraa: Failed to do IO pinmuxing on null/unkown platform\n");
        } else {
            imraa_handle_IO(jobj);
        }
    }
    fclose(fh);
    json_object_put(jobj);
    free(buffer);
    return EXIT_SUCCESS;
}
