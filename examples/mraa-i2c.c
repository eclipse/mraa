/*
 * Author: Henry Bruce <henry.bruce@intel.com>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mraa/i2c.h"

#include "mraa_internal_types.h"

extern mraa_board_t* plat;

void
print_version()
{
    fprintf(stdout, "Version %s on %s", mraa_get_version(), mraa_get_platform_name());
    if (plat != NULL && plat->sub_platform != NULL)
        fprintf(stdout, " with %s", plat->sub_platform->platform_name);
    fprintf(stdout, "\n");
}

void
print_help()
{
    fprintf(stdout, "version                   Get mraa version and board name\n");
    fprintf(stdout, "list                      List available busses\n");
    fprintf(stdout, "detect bus                List detected devices on specified bus\n");
    fprintf(stdout, "get bus device reg        Get value from specified device register\n");
    fprintf(stdout, "set bus device reg value  Set specified device register to value\n");
}

void
print_command_error()
{
    fprintf(stdout, "Invalid command, options are:\n");
    print_help();
}

void
print_bus(mraa_board_t* board)
{
    int i, bus;
    for (i = 0; i < board->i2c_bus_count; ++i) {
        char* busType;
        switch (board->platform_type) {
            case MRAA_INTEL_GALILEO_GEN1:
            case MRAA_INTEL_GALILEO_GEN2:
            case MRAA_INTEL_EDISON_FAB_C:
            case MRAA_INTEL_DE3815:
            case MRAA_INTEL_MINNOWBOARD_MAX:
            case MRAA_RASPBERRY_PI:
            case MRAA_BEAGLEBONE:
            case MRAA_BANANA:
                bus = i;
                busType = "linux";
                break;
            case MRAA_FTDI_FT4222:
                busType = "ft4222";
                bus = mraa_get_sub_platform_id(i);
                break;
            default:
                busType = "unknown";
                break;
        }
        int id = board->i2c_bus[bus].bus_id;
        fprintf(stdout, "Bus %3d: id=%02d type=%s ", bus, id, busType);
        if (i == board->def_i2c_bus)
            fprintf(stdout, " default");
        if (id == -1)
            fprintf(stdout, " disabled");

        fprintf(stdout, "\n");
    }
}

void
print_busses()
{
    print_bus(plat);
    if (mraa_has_sub_platform())
        print_bus(plat->sub_platform);
}

mraa_result_t
i2c_get(int bus, uint8_t device_address, uint8_t register_address, uint8_t* data)
{
    mraa_result_t status = MRAA_SUCCESS;
    mraa_i2c_context i2c = mraa_i2c_init(bus);
    if (i2c == NULL) {
        return MRAA_ERROR_NO_RESOURCES;
    }
    status = mraa_i2c_address(i2c, device_address);
    if (status != MRAA_SUCCESS) {
        goto i2c_get_exit;
    }
    status = mraa_i2c_write_byte(i2c, register_address);
    if (status != MRAA_SUCCESS) {
        goto i2c_get_exit;
    }
    status = mraa_i2c_read(i2c, data, 1) == 1 ? MRAA_SUCCESS : MRAA_ERROR_UNSPECIFIED;
    if (status != MRAA_SUCCESS) {
        goto i2c_get_exit;
    }
i2c_get_exit:
    mraa_i2c_stop(i2c);
    return status;
}

mraa_result_t
i2c_set(int bus, uint8_t device_address, uint8_t register_address, uint8_t data)
{
    mraa_result_t status = MRAA_SUCCESS;
    mraa_i2c_context i2c = mraa_i2c_init(bus);
    if (i2c == NULL) {
        return MRAA_ERROR_NO_RESOURCES;
    }
    status = mraa_i2c_address(i2c, device_address);
    if (status != MRAA_SUCCESS) {
        fprintf(stderr, "Could not set i2c device address\n");
        goto i2c_set_exit;
    }
    status = mraa_i2c_write_byte_data(i2c, data, register_address);
    if (status != MRAA_SUCCESS) {
        fprintf(stderr, "Could not write to i2c register. Status = %d\n", status);
        goto i2c_set_exit;
    }
i2c_set_exit:
    mraa_i2c_stop(i2c);
    return status;
}

void
i2c_detect_devices(int bus)
{
    mraa_result_t status = MRAA_SUCCESS;
    mraa_i2c_context i2c = mraa_i2c_init(bus);
    if (i2c == NULL) {
        return;
    }
    int addr;
    for (addr = 0x0; addr < 0x80; ++addr) {
        uint8_t value;
        if ((addr) % 16 == 0)
            printf("%02x: ", addr);
        if (i2c_get(bus, addr, 0, &value) == MRAA_SUCCESS)
            printf("%02x ", addr);
        else
            printf("-- ");
        if ((addr + 1) % 16 == 0)
            printf("\n");
    }
}

int
process_command(int argc, char** argv)
{
    if (strcmp(argv[1], "help") == 0) {
        print_help();
        return 0;
    } else if (strcmp(argv[1], "version") == 0) {
        print_version();
        return 0;
    } else if (strcmp(argv[1], "list") == 0) {
        print_busses();
        return 0;
    } else if (strcmp(argv[1], "detect") == 0) {
        if (argc == 3) {
            int bus = strtol(argv[2], NULL, 0);
            i2c_detect_devices(bus);
            return 0;
        } else {
            print_command_error();
            return 1;
        }
    } else if ((strcmp(argv[1], "get") == 0)) {
        if (argc == 5) {
            int bus = strtol(argv[2], NULL, 0);
            uint8_t device_address = strtol(argv[3], NULL, 0);
            uint8_t register_address = strtol(argv[4], NULL, 0);
            // fprintf(stdout, "Device %02X, Register = %02X\n", device_address, register_address);
            uint8_t data;
            if (i2c_get(bus, device_address, register_address, &data) == MRAA_SUCCESS) {
                fprintf(stdout, "Register %#02X = %#02X\n", register_address, data);
                return 0;
            } else {
                fprintf(stdout, "i2c get failed\n");
                return 1;
            }
        } else {
            print_command_error();
            return 1;
        }
    } else if ((strcmp(argv[1], "set") == 0)) {
        if (argc == 6) {
            int bus = strtol(argv[2], NULL, 0);
            uint8_t device_address = strtol(argv[3], NULL, 0);
            uint8_t register_address = strtol(argv[4], NULL, 0);
            uint8_t value = strtol(argv[5], NULL, 0);
            fprintf(stdout, "Device %02X, Register = %02X, Value = %02X\n", device_address,
                    register_address, value);
            if (i2c_set(bus, device_address, register_address, value) != MRAA_SUCCESS) {
                fprintf(stdout, "i2c set failed\n");
                return 0;
            }
            return 1;
        } else {
            print_command_error();
            return 1;
        }
    } else {
        print_command_error();
        return 1;
    }
}

void
run_interactive_mode()
{
    char command[80];
    while (1) {
        int i, argc = 1;
        char* argv[32];
        char* arg;
        argv[0] = "mraa-i2c";
        fprintf(stdout, "Command: ");
        fgets(command, 80, stdin);
        command[strlen(command) - 1] = 0;
        if (strcmp(command, "q") == 0)
            return;
        char* str = strtok(command, " ");
        while (str != NULL) {
            arg = malloc(strlen(str) + 1);
            argv[argc++] = strcpy(arg, str);
            str = strtok(NULL, " ");
        }
        process_command(argc, argv);
        for (i = 1; i < argc; ++i)
            free(argv[i]);
    }
}

int
main(int argc, char** argv)
{
    mraa_set_log_level(7);
    if (argc == 1) {
        run_interactive_mode();
        return 0;
    } else
        return process_command(argc, argv);
}
