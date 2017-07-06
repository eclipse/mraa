/*
 * Author: Manivannan Sadhasivam <manivannan.sadhsivam@linaro.org>
 * Copyright (c) 2017 Linaro Ltd.
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
 *
 * TODO: Add functions for gpio events and accessing multiple gpio's
 *
 */

#include "mraa/gpio_chardev.h"
#include "mraa_internal.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/gpio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

void
print_version()
{
    fprintf(stdout, "Version %s on %s\n", mraa_get_version(), mraa_get_platform_name());
}

void
print_help()
{
    fprintf(stdout, "list pins         List pins for a gpiochip\n");
    fprintf(stdout, "set pin level     Set pin to level (0/1)\n");
    fprintf(stdout, "get pin           Get pin level\n");
    fprintf(stdout, "version           Get mraa version and board name\n");
}

void
print_command_error()
{
    fprintf(stdout, "Invalid command, options are:\n");
    print_help();
}

int list_pins(int chip) // TODO: Check for valid gpiochip
{
    mraa_gpio_char_context gpio;
    gpio = mraa_gpio_char_init(chip, 0);

    fprintf(stdout, "Total number of pins in gpiochip %d is: %d\n", chip, gpio->cinfo.lines);

    close(gpio->chip_fd);
    free(gpio);
    return 0;
}

mraa_result_t
gpio_set(int chip, int pin, int level)
{
    mraa_result_t res;
    mraa_gpio_char_context gpio;

    gpio = mraa_gpio_char_init(chip, pin);
    if (gpio == NULL)
        return MRAA_ERROR_INVALID_RESOURCE;

    res = mraa_gpio_char_set_output(gpio, MRAA_GPIO_STATE_ACTIVE_HIGH, MRAA_GPIO_OUTPUT_OPEN_SOURCE,
                                    MRAA_GPIO_DEFAULT_HIGH);
    if (res == MRAA_SUCCESS) {
        res = mraa_gpio_char_write(gpio, level);
    }

    return mraa_gpio_char_close(gpio);
}

mraa_result_t
gpio_get(int chip, int pin, int* level)
{
    mraa_result_t res;
    mraa_gpio_char_context gpio;

    gpio = mraa_gpio_char_init(chip, pin);
    if (gpio == NULL)
        return MRAA_ERROR_INVALID_RESOURCE;

    res = mraa_gpio_char_set_input(gpio, MRAA_GPIO_STATE_ACTIVE_HIGH);
    if (res == MRAA_SUCCESS) {
        *level = mraa_gpio_char_read(gpio);
    }

    return mraa_gpio_char_close(gpio);
}

int
main(int argc, char* argv[])
{
    if (argc == 1) {
        print_command_error();
    }

    if (argc > 1) {
        if (strcmp(argv[1], "list") == 0) {
            if (argc == 3) {
                list_pins(atoi(argv[2]));
            } else {
                print_command_error();
            }
        } else if (strcmp(argv[1], "help") == 0) {
            print_help();
        } else if (strcmp(argv[1], "version") == 0) {
            print_version();
        } else if (strcmp(argv[1], "set") == 0) {
            if (argc == 5) {
                int chip = atoi(argv[2]);
                int pin = atoi(argv[3]);
                int value = atoi(argv[4]);
                if (gpio_set(chip, pin, value) != MRAA_SUCCESS)
                    fprintf(stdout, "gpio set failed for pin: %d\n", pin);
            } else {
                print_command_error();
            }
        } else if (strcmp(argv[1], "get") == 0) {
            if (argc == 4) {
                int chip = atoi(argv[2]);
                int pin = atoi(argv[3]);
                int level;
                if (gpio_get(chip, pin, &level) == MRAA_SUCCESS) {
                    fprintf(stdout, "Pin %d = %d\n", pin, level);
                } else {
                    fprintf(stdout, "gpio get failed for pin: %d\n", pin);
                }
            } else {
                print_command_error();
            }
        }
    }

    return 0;
}
