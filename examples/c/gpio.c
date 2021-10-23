/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Contributors: Alex Tereschenko <alext.mkrs@gmail.com>
 * Contributors: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 * Example usage: Toggles GPIO's 23 and 24 recursively. Press Ctrl+C to exit
 *
 */

/* standard headers */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* mraa header */
#include "mraa/gpio.h"

/* gpio declaration */
#define GPIO_PIN_1 23
#define GPIO_PIN_2 24

volatile sig_atomic_t flag = 1;

void
sig_handler(int signum)
{
    if (signum == SIGINT) {
        fprintf(stdout, "Exiting...\n");
        flag = 0;
    }
}

int
main(void)
{
    mraa_result_t status = MRAA_SUCCESS;
    mraa_gpio_context gpio_1, gpio_2;

    /* install signal handler */
    signal(SIGINT, sig_handler);

    /* initialize mraa for the platform (not needed most of the times) */
    mraa_init();

    //! [Interesting]
    /* initialize GPIO pin */
    gpio_1 = mraa_gpio_init(GPIO_PIN_1);
    if (gpio_1 == NULL) {
        fprintf(stderr, "Failed to initialize GPIO %d\n", GPIO_PIN_1);
        mraa_deinit();
        return EXIT_FAILURE;
    }

    /* initialize GPIO pin */
    gpio_2 = mraa_gpio_init(GPIO_PIN_2);
    if (gpio_2 == NULL) {
        fprintf(stderr, "Failed to initialize GPIO %d\n", GPIO_PIN_2);
        mraa_deinit();
        return EXIT_FAILURE;
    }

    /* set GPIO to output */
    status = mraa_gpio_dir(gpio_1, MRAA_GPIO_OUT);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }

    /* set GPIO to output */
    status = mraa_gpio_dir(gpio_2, MRAA_GPIO_OUT);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }

    /* toggle both GPIO's */
    while (flag) {
        status = mraa_gpio_write(gpio_1, 1);
        if (status != MRAA_SUCCESS) {
            goto err_exit;
        }

        status = mraa_gpio_write(gpio_2, 0);
        if (status != MRAA_SUCCESS) {
            goto err_exit;
        }

        sleep(1);

        status = mraa_gpio_write(gpio_1, 0);
        if (status != MRAA_SUCCESS) {
            goto err_exit;
        }

        status = mraa_gpio_write(gpio_2, 1);
        if (status != MRAA_SUCCESS) {
            goto err_exit;
        }

        sleep(1);
    }

    /* release gpio's */
    status = mraa_gpio_close(gpio_1);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }

    /* close GPIO */
    status = mraa_gpio_close(gpio_2);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }

    //! [Interesting]
    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_SUCCESS;

err_exit:
    mraa_result_print(status);

    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_FAILURE;
}
