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

char* board_name;

void
print_version() {
    fprintf(stdout, "Version %s on %s\n", mraa_get_version(), board_name);
}

void
print_help() {
    fprintf(stdout, "list              List pins\n");
    fprintf(stdout, "set pin level     Set pin to level (0/1)\n");
    fprintf(stdout, "get pin           Get pin level\n");
    fprintf(stdout, "version           Get mraa version and board name\n");
}

void
print_command_error() {
    fprintf(stdout, "Invalid command, options are:\n");
    fprintf(stdout, "list              List pins\n");
    fprintf(stdout, "set pin level     Set pin to level (0/1)\n");
    fprintf(stdout, "get pin           Get pin level\n");
    fprintf(stdout, "version           Get mraa version and board name\n");
}

void
list_pins() {
    int pin_count = mraa_get_pin_count();
    if (pin_count == 0) {
        fprintf(stdout, "No Pins\n");
        return;
    }
    int i;
    for (i = 0; i < pin_count; ++i) {
        fprintf(stdout, "%02d ", i);
        if (mraa_pin_mode_test(i, MRAA_PIN_GPIO))
            fprintf(stdout, "GPIO");
        if (mraa_pin_mode_test(i, MRAA_PIN_I2C))
            fprintf(stdout, "I2C");
        if (mraa_pin_mode_test(i, MRAA_PIN_SPI))
            fprintf(stdout, "SPI");
        if (mraa_pin_mode_test(i, MRAA_PIN_PWM))
            fprintf(stdout, "PWM");
        if (mraa_pin_mode_test(i, MRAA_PIN_UART))
            fprintf(stdout, "UART");
        fprintf(stdout, "\n");
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
    board_name = mraa_get_platform_name();
    if (board_name == NULL) {
        board_name = "Error";
    }

    if (argc == 1) {
        print_command_error();
    }

    if (argc > 1) {
        if (strcmp(argv[1], "list") == 0) {
            list_pins();
        } else if (strcmp(argv[1], "help") == 0) {
            print_help();
        } else if (strcmp(argv[1], "version") == 0) {
            print_version();
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
