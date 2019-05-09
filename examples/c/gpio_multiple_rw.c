/*
 * Copyright (c) 2017 Mihai Stefanescu
 * SPDX-License-Identifier: MIT
 *
 * This currently works only on Intel Joule Platform.
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "mraa/gpio.h"

int running = 0;

void
sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("Closing down nicely\n");
        running = -1;
    }
}

int main(int argc, char** argv)
{
    mraa_result_t r = MRAA_SUCCESS;
    /* These are Joule's onboard LED's. */
    int default_joule_leds[] = {100, 101, 102, 103};
    int *gpio_pins;
    int num_pins;
    int *input_values, *output_values;

    printf("Provide int arg(s) if you want to toggle gpio pins other than Joule's onboard LED's\n");

    if (argc < 2) {
        gpio_pins = default_joule_leds;
        num_pins = 4;
    } else {
        num_pins = argc - 1;
        gpio_pins = malloc(num_pins * sizeof(int));

        for (int i = 0; i < num_pins; ++i) {
            gpio_pins[i] = strtol(argv[i+1], NULL, 10);
        }
    }

    /* Allocate input and output values arrays. */
    input_values = malloc(num_pins * sizeof(int));
    output_values = malloc(num_pins * sizeof(int));

    mraa_init();
    fprintf(stdout, "MRAA Version: %s\nStarting program...\n", mraa_get_version());

    mraa_gpio_context gpio = mraa_gpio_init_multi(gpio_pins, num_pins);
    if (gpio == NULL) {
        fprintf(stderr, "Error during gpio initialization\n");
        exit(1);
    }

    r = mraa_gpio_dir(gpio, MRAA_GPIO_OUT);
    if (r != MRAA_SUCCESS) {
        mraa_result_print(r);
    }

    signal(SIGINT, sig_handler);

    /* Set input values first. */
    memset(input_values, 0, num_pins * sizeof(int));

    while (running == 0) {
        r = mraa_gpio_write_multi(gpio, input_values);

        sleep(1);

        if (r != MRAA_SUCCESS) {
            mraa_result_print(r);
        } else {
            r = mraa_gpio_read_multi(gpio, output_values);
            if (r != MRAA_SUCCESS) {
                mraa_result_print(r);
            }
        }

        for (int i = 0; i < num_pins; ++i) {
            input_values[i] = (input_values[i] + 1) % 2;
        }
    }

    memset(input_values, 0, num_pins * sizeof(int));
    mraa_gpio_write_multi(gpio, input_values);

    r = mraa_gpio_close(gpio);
    if (r != MRAA_SUCCESS) {
        mraa_result_print(r);
    }

    /* Cleanup. */
    if (argc >= 2) {
       free(gpio_pins);
    }
    free(input_values);
    free(output_values);

    return r;
}