/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
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
#include <string.h>
#include <unistd.h>

#include "mraa/gpio.h"

extern mraa_board_t* plat;

char board_name[] = "Some weird devboard that isn't recognised...";

void
print_version() {
    fprintf(stdout, "Version %s on %s\n", mraa_get_version(), board_name);
}

void
print_help() {
    fprintf(stdout, "list              List pins\n");
    fprintf(stdout, "set pin level     Set pin to level (0/1)\n");
    fprintf(stdout, "get pin           Get pin level\n");
}

void
print_command_error() {
    fprintf(stdout, "Invalid command, options are:\n");
    fprintf(stdout, "list              List pins\n");
    fprintf(stdout, "set pin level     Set pin to level (0/1)\n");
    fprintf(stdout, "get pin           Get pin level\n");
}

void
list_pins() {
    const mraa_board_t* platformInfo = plat;
    int i;
    for (i = 0; i < platformInfo->phy_pin_count; ++i) {
        const char *mraa_name = platformInfo->pins[i].name;
        if (strcmp(mraa_name, "INVALID") != 0) {
            char name[9];
            strncpy(name, mraa_name, 8);
            if (name[7] != '\0');
                name[8] = '\0';
            mraa_pincapabilities_t caps = platformInfo->pins[i].capabilites;
            fprintf(stdout, "%02d %-8s  ", i, name);
            if (caps.gpio)
                fprintf(stdout, "GPIO");
            if (caps.i2c)
                fprintf(stdout, "I2C");
            if (caps.spi)
                fprintf(stdout, "SPI");
            if (caps.pwm)
                fprintf(stdout, "PWM");
            if (caps.uart)
                fprintf(stdout, "UART");
            fprintf(stdout, "\n");
        }
    }
}

mraa_result_t
gpio_set(int pin, int level) {
    mraa_gpio_context gpio = mraa_gpio_init(pin);
    if (gpio != NULL) {
        mraa_gpio_dir(gpio, MRAA_GPIO_OUT);
        mraa_gpio_write(gpio, level);
        return MRAA_SUCCESS;
    } else
        return MRAA_ERROR_INVALID_RESOURCE;
}

mraa_result_t
gpio_get(int pin, int* level) {
    mraa_gpio_context gpio = mraa_gpio_init(pin);
    if (gpio != NULL) {
        mraa_gpio_dir(gpio, MRAA_GPIO_IN);
        *level = mraa_gpio_read(gpio);
        return MRAA_SUCCESS;
    } else
        return MRAA_ERROR_INVALID_RESOURCE;
}

int
main(int argc, char **argv)
{
    mraa_platform_t platform = mraa_get_platform_type();

    switch (platform) {
        case MRAA_INTEL_GALILEO_GEN1:
            strcpy(board_name, "Intel Galileo Gen1");
            break;
        case MRAA_INTEL_GALILEO_GEN2:
            strcpy(board_name, "Intel Galileo Gen2");
            break;
        case MRAA_INTEL_EDISON_FAB_C:
            strcpy(board_name, "Intel Edison");
            break;
        case MRAA_INTEL_MINNOWBOARD_MAX:
            strcpy(board_name, "Intel Minnowboard Max");
            break;
        default:
            fprintf(stdout, "Unknown platform code %d\n", platform);
            return 1;
    }


    if (argc == 1) {
        print_command_error();
    }

    if (argc > 1) {
        if (strcmp(argv[1], "list") == 0) {
            list_pins();
        } else if (strcmp(argv[1], "help") == 0) {
            print_help();
        } else if (strcmp(argv[1], "set") == 0) {
            if (argc == 4) {
                int pin = atoi(argv[2]);
                if (gpio_set(pin, atoi(argv[3])) != MRAA_SUCCESS)
                    fprintf(stdout, "Could not initialize gpio %d\n", pin);
            } else
                print_command_error();
        } else if (strcmp(argv[1], "get") == 0) {
            if (argc == 3) {
                int pin = atoi(argv[2]);
                int level;
                if (gpio_get(pin, &level) == MRAA_SUCCESS)
                    fprintf(stdout, "Pin %d = %d\n", pin, level);
                else
                    fprintf(stdout, "Could not initialize gpio %d\n", pin);
            } else
                print_command_error();
        } else {
            print_command_error();
        }
    }
    return 0;
}
