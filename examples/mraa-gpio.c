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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mraa/gpio.h"

struct gpio_source {
    int pin;
    mraa_gpio_context context;
};

void
print_version()
{
    fprintf(stdout, "Version %s on %s\n", mraa_get_version(), mraa_get_platform_name());
}

void
print_help()
{
    fprintf(stdout, "list              List pins\n");
    fprintf(stdout, "set pin level     Set pin to level (0/1)\n");
    fprintf(stdout, "setraw pin level  Set pin to level (0/1) via mmap (if available)\n");
    fprintf(stdout, "get pin           Get pin level\n");
    fprintf(stdout, "getraw pin        Get pin level via mmap (if available)\n");
    fprintf(stdout, "monitor pin       Monitor pin level changes\n");
    fprintf(stdout, "version           Get mraa version and board name\n");
}

void
print_command_error()
{
    fprintf(stdout, "Invalid command, options are:\n");
    print_help();
}

int
list_platform_pins()
{
    int pin_count = mraa_get_pin_count();
    int i;
    for (i = 0; i < pin_count; ++i) {
        if (strcmp(mraa_get_pin_name(i), "INVALID") != 0) {
            int pin_id = mraa_is_sub_platform_selected() ? mraa_use_sub_platform(i) : i;
            fprintf(stdout, "%02d ", pin_id);
            fprintf(stdout, "%*s: ", (MRAA_PIN_NAME_SIZE - 1), mraa_get_pin_name(i));
            if (mraa_pin_mode_test(i, MRAA_PIN_GPIO))
                fprintf(stdout, "GPIO ");
            if (mraa_pin_mode_test(i, MRAA_PIN_I2C))
                fprintf(stdout, "I2C  ");
            if (mraa_pin_mode_test(i, MRAA_PIN_SPI))
                fprintf(stdout, "SPI  ");
            if (mraa_pin_mode_test(i, MRAA_PIN_PWM))
                fprintf(stdout, "PWM  ");
            if (mraa_pin_mode_test(i, MRAA_PIN_AIO))
                fprintf(stdout, "AIO  ");
            if (mraa_pin_mode_test(i, MRAA_PIN_UART))
                fprintf(stdout, "UART ");
            fprintf(stdout, "\n");
        }
    }
    return pin_count;    
}

int
list_pins()
{
    int pin_count = 0;
    mraa_select_main_platform();
    pin_count += list_platform_pins();
    mraa_select_sub_platform();
    pin_count += list_platform_pins();    
    if (pin_count == 0) {
        fprintf(stdout, "No Pins\n");
    }
    mraa_select_main_platform();        
}


mraa_result_t
gpio_set(int pin, int level, mraa_boolean_t raw)
{
    mraa_gpio_context gpio = mraa_gpio_init(pin);
    if (gpio != NULL) {
        mraa_gpio_dir(gpio, MRAA_GPIO_OUT);
        if (raw != 0) {
            if (mraa_gpio_use_mmaped(gpio, 1) != MRAA_SUCCESS) {
                fprintf(stdout,
                        "mmapped access to gpio %d not supported, falling back to normal mode\n", pin);
            }
        }
        mraa_gpio_write(gpio, level);
        return MRAA_SUCCESS;
    }
    return MRAA_ERROR_INVALID_RESOURCE;
}

mraa_result_t
gpio_get(int pin, int* level, mraa_boolean_t raw)
{
    mraa_gpio_context gpio = mraa_gpio_init(pin);
    if (gpio != NULL) {
        mraa_gpio_dir(gpio, MRAA_GPIO_IN);
        if (raw != 0) {
            if (mraa_gpio_use_mmaped(gpio, 1) != MRAA_SUCCESS) {
                fprintf(stdout,
                        "mmapped access to gpio %d not supported, falling back to normal mode\n", pin);
            }
        }
        *level = mraa_gpio_read(gpio);
        return MRAA_SUCCESS;
    }
    return MRAA_ERROR_INVALID_RESOURCE;
}


void
gpio_isr_handler(void* args)
{
    struct gpio_source* gpio_info = (struct gpio_source*) args;
    int level = mraa_gpio_read(gpio_info->context);
    fprintf(stdout, "Pin %d = %d\n", gpio_info->pin, level);
}

mraa_result_t
gpio_isr_start(struct gpio_source* gpio_info)
{
    gpio_info->context = mraa_gpio_init(gpio_info->pin);
    if (gpio_info->context != NULL) {
        mraa_result_t status = mraa_gpio_dir(gpio_info->context, MRAA_GPIO_IN);
        if (status == MRAA_SUCCESS) {
            status = mraa_gpio_isr(gpio_info->context, MRAA_GPIO_EDGE_BOTH, &gpio_isr_handler, gpio_info);
        }
        return status;
    } else {
        return MRAA_ERROR_INVALID_RESOURCE;
    }
}


mraa_result_t
gpio_isr_stop(struct gpio_source* gpio_info)
{
    mraa_gpio_isr_exit(gpio_info->context);
    mraa_gpio_close(gpio_info->context);
    return MRAA_SUCCESS;
}


int
main(int argc, char** argv)
{
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
        } else if ((strcmp(argv[1], "set") == 0) || (strcmp(argv[1], "setraw") == 0)) {
            if (argc == 4) {
                int pin = atoi(argv[2]);
                mraa_boolean_t rawmode = strcmp(argv[1], "setraw") == 0;
                if (gpio_set(pin, atoi(argv[3]), rawmode) != MRAA_SUCCESS)
                    fprintf(stdout, "Could not initialize gpio %d\n", pin);
            } else {
                print_command_error();
            }
        } else if ((strcmp(argv[1], "get") == 0) || (strcmp(argv[1], "getraw") == 0)) {
            if (argc == 3) {
                int pin = atoi(argv[2]);
                int level;
                mraa_boolean_t rawmode = strcmp(argv[1], "getraw") == 0;
                if (gpio_get(pin, &level, rawmode) == MRAA_SUCCESS) {
                    fprintf(stdout, "Pin %d = %d\n", pin, level);
                } else {
                    fprintf(stdout, "Could not initialize gpio %d\n", pin);
                }
            } else {
                print_command_error();
            }
        } else if (strcmp(argv[1], "monitor") == 0) {
            if (argc == 3) {
                int pin = atoi(argv[2]);
                struct gpio_source gpio_info;
                gpio_info.pin = pin;
                if (gpio_isr_start(&gpio_info) == MRAA_SUCCESS) {
                    fprintf(stdout, "Monitoring level changes to pin %d. Press RETURN to exit.\n", pin);
                    gpio_isr_handler(&gpio_info);
                    char aux = 0;
                    do {
                        fflush(stdin);
                        fscanf(stdin, "%c", &aux);
                    } while (aux != '\n');
                    gpio_isr_stop(&gpio_info);
                } else {
                    fprintf(stdout, "Failed to register ISR for pin %d\n", pin);
                }
            } else {
                print_command_error();
            }
        } else {
            print_command_error();
        }
    }
    return 0;
}
