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
    fprintf(stdout, "Version %s on %s\n", mraa_get_version(), mraa_get_platform_name());
}

void
print_help()
{
    fprintf(stdout, "version                   Get mraa version and board name\n");
    fprintf(stdout, "list                      List available busses\n");
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
print_busses()
{
    int bus;
    for (bus = 0; bus < plat->i2c_bus_count; ++bus) {
        char* busType;
        switch (plat->i2c_bus[bus].drv_type) {
        case MRAA_DRV_API_STD:
            busType = "stdapi";
            break;
        case MRAA_DRV_API_FT4222:
            busType = "ft4222";
            break;
        default:
            busType = "unknown";
            break;
        }
        fprintf(stdout, "Bus %2d: id=%02d type=%s ", bus, plat->i2c_bus[bus].bus_id, busType);
        if (bus == plat->def_i2c_bus)
            fprintf(stdout, " default", bus);
        fprintf(stdout, "\n", bus);
    }
}


mraa_result_t
i2c_get(int bus, uint8_t device_address, uint8_t register_address, uint8_t* data)
{
    mraa_result_t status = MRAA_SUCCESS;
    mraa_i2c_context i2c = mraa_i2c_init(bus);
    if (i2c == NULL) {
        status = MRAA_ERROR_NO_RESOURCES;
        fprintf(stdout, "Could not initialize i2c\n");
        goto i2c_get_exit;
    }
    status = mraa_i2c_address(i2c, device_address);
    if (status != MRAA_SUCCESS) {
        fprintf(stdout, "Could not set i2c device address\n");
        goto i2c_get_exit;
    }
    status = mraa_i2c_write_byte(i2c, register_address);
    if (status != MRAA_SUCCESS) {
        fprintf(stdout, "Could not set i2c register address. Status = %d\n", status);
        goto i2c_get_exit;
    }
    status = mraa_i2c_read(i2c, data, 1) == 1 ? MRAA_SUCCESS : MRAA_ERROR_UNSPECIFIED;
    if (status != MRAA_SUCCESS) {
        fprintf(stdout, "i2c read failed\n");
       goto i2c_get_exit;
    }
i2c_get_exit:
    if (i2c != NULL)
    mraa_i2c_stop(i2c);
    return status;
}


mraa_result_t
i2c_set(int bus, uint8_t device_address, uint8_t register_address, uint8_t data)
{
    mraa_result_t status = MRAA_SUCCESS;
    mraa_i2c_context i2c = mraa_i2c_init(bus);
    if (i2c == NULL) {
        status = MRAA_ERROR_NO_RESOURCES;
        fprintf(stdout, "Could not initialize i2c\n");
        goto i2c_set_exit;
    }
    status = mraa_i2c_address(i2c, device_address);
    if (status != MRAA_SUCCESS) {
        fprintf(stdout, "Could not set i2c device address\n");
        goto i2c_set_exit;
    }
    status = mraa_i2c_write_byte_data(i2c, data, register_address);
    if (status != MRAA_SUCCESS) {
        fprintf(stdout, "Could not set i2c register address. Status = %d\n", status);
        goto i2c_set_exit;
    }
/*
    status = mraa_i2c_write_byte(i2c, data);
    if (status != MRAA_SUCCESS) {
        fprintf(stdout, "Could not set value. Status = %d\n", status);
        goto i2c_set_exit;
  }
*/
i2c_set_exit:
    if (i2c != NULL)
    mraa_i2c_stop(i2c);
    return status;
}



int
main(int argc, char** argv)
{
    mraa_set_log_level(7);
    if (argc == 1) {
        print_command_error();
    }

    if (argc > 1) {
        if (strcmp(argv[1], "help") == 0) {
            print_help();
        } else if (strcmp(argv[1], "version") == 0) {
            print_version();
        } else if (strcmp(argv[1], "list") == 0) {
            print_busses();
        } else if ((strcmp(argv[1], "get") == 0)) {
            if (argc == 5) {
                uint8_t bus = strtol(argv[2], NULL, 0);
                uint8_t device_address = strtol(argv[3], NULL, 0);
                uint8_t register_address = strtol(argv[4], NULL, 0);
                // fprintf(stdout, "Device %02X, Register = %02X\n", device_address, register_address);
                uint8_t data;
                if (i2c_get(bus, device_address, register_address, &data) == MRAA_SUCCESS) {
                    fprintf(stdout, "Register %#02X = %#02X\n", register_address, data);
                } else {
                    fprintf(stdout, "i2c get failed\n");
                }
            } else {
                print_command_error();
            }
        } else if ((strcmp(argv[1], "set") == 0)) {
            if (argc == 6) {
                uint8_t bus = strtol(argv[2], NULL, 0);
                uint8_t device_address = strtol(argv[3], NULL, 0);
                uint8_t register_address = strtol(argv[4], NULL, 0);
                uint8_t value = strtol(argv[5], NULL, 0);
                fprintf(stdout, "Device %02X, Register = %02X, Value = %02X\n", device_address, register_address, value);
                if (i2c_set(bus, device_address, register_address, value) != MRAA_SUCCESS) {
                    fprintf(stdout, "i2c set failed\n");
                }
            } else {
                print_command_error();
            }
        }

    }
    return 0;
}

