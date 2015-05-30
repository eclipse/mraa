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
    int bus_index;
    int bus;
    for (bus = 0; bus < board->i2c_bus_count; ++bus) {
        char* busType;
        switch (mraa_get_platform_type()) {
        case MRAA_INTEL_GALILEO_GEN1:
        case MRAA_INTEL_GALILEO_GEN2:
        case MRAA_INTEL_EDISON_FAB_C:
        case MRAA_INTEL_DE3815:
        case MRAA_INTEL_MINNOWBOARD_MAX:
        case MRAA_RASPBERRY_PI:
        case MRAA_BEAGLEBONE:
        case MRAA_BANANA:
            bus_index = bus;
            busType = "stdapi";
            break;
        case MRAA_FTDI_FT4222:
            busType = "ft4222";
            bus_index = MRAA_USE_SUB_PLATFORM(bus);
            break;
        default:
            busType = "unknown";
            break;
        }
        fprintf(stdout, "Bus %2d: id=%02d type=%s ", bus_index, plat->i2c_bus[bus].bus_id, busType);
        if (bus == plat->def_i2c_bus)
            fprintf(stdout, " default", bus);
        fprintf(stdout, "\n", bus);
    }
}


void
print_busses()
{
   print_bus(plat);
   if (plat->sub_platform != NULL)
      print_bus(plat->sub_platform);
}



mraa_result_t
i2c_get(int bus, uint8_t device_address, uint8_t register_address, uint8_t* data)
{
    mraa_result_t status = MRAA_SUCCESS;
    mraa_i2c_context i2c = mraa_i2c_init(bus);
    if (i2c == NULL) {
        status = MRAA_ERROR_NO_RESOURCES;
        // fprintf(stdout, "Could not initialize i2c\n");
        goto i2c_get_exit;
    }
    status = mraa_i2c_address(i2c, device_address);
    if (status != MRAA_SUCCESS) {
        // fprintf(stdout, "Could not set i2c device address\n");
        goto i2c_get_exit;
    }
    status = mraa_i2c_write_byte(i2c, register_address);
    if (status != MRAA_SUCCESS) {
        // fprintf(stdout, "Could not set i2c register address. Status = %d\n", status);
        goto i2c_get_exit;
    }
    status = mraa_i2c_read(i2c, data, 1) == 1 ? MRAA_SUCCESS : MRAA_ERROR_UNSPECIFIED;
    if (status != MRAA_SUCCESS) {
        // fprintf(stdout, "i2c read failed\n");
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
        fprintf(stdout, "Could not write to i2c register. Status = %d\n", status);
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


void i2c_detect_devices(int bus)
{
    mraa_result_t status = MRAA_SUCCESS;
    mraa_i2c_context i2c = mraa_i2c_init(bus);
    int addr;
	for (addr=0x0; addr < 0x80; ++addr) {
		uint8_t value;
		if ((addr) % 16 == 0)
		   printf("%02x: ", addr);
		if (i2c_get(bus, addr, 0, &value) == MRAA_SUCCESS)
			printf("%02x ", addr);
		else
			printf("-- ", addr);
		if ((addr + 1) % 16 == 0)
		   printf("\n");
	}
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
        } else if (strcmp(argv[1], "detect") == 0) {
            if (argc == 3) {
                int bus = strtol(argv[2], NULL, 0);
                i2c_detect_devices(bus);
            } else {
                print_command_error();
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
                } else {
                    fprintf(stdout, "i2c get failed\n");
                }
            } else {
                print_command_error();
            }
        } else if ((strcmp(argv[1], "set") == 0)) {
            if (argc == 6) {
                int bus = strtol(argv[2], NULL, 0);
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

