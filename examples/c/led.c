/*
 * Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 * Copyright (c) 2018, Linaro Ltd.
 *
 * SPDX-License-Identifier: MIT
 *
 * Example usage: Reads maximum brightness value for user led and turns it
 *                on/off depending on current state. Then sets led trigger
 *                to heartbeat.
 *
 */

/* standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* mraa header */
#include "mraa/led.h"

/* LED name */
#define USER_LED 0

/* trigger type */
#define LED_TRIGGER "heartbeat"

int
main(void)
{
    mraa_result_t status = MRAA_SUCCESS;
    mraa_led_context led;
    int val;

    /* initialize mraa for the platform (not needed most of the time) */
    mraa_init();

    //! [Interesting]
    /* initialize LED */
    led = mraa_led_init(USER_LED);
    if (led == NULL) {
        fprintf(stderr, "Failed to initialize LED\n");
        mraa_deinit();
        return EXIT_FAILURE;
    }

    /* read maximum brightness */
    val = mraa_led_read_max_brightness(led);
    fprintf(stdout, "maximum brightness value for LED is: %d\n", val);
    if (val >= 1) {
        val = 0;
    } else {
        /* never reached mostly */
        val = 1;
    }

    /* turn LED on/off depending on max_brightness value */
    status = mraa_led_set_brightness(led, val);
    if (status != MRAA_SUCCESS) {
        fprintf(stderr, "unable to set LED brightness\n");
        goto err_exit;
    }

    /* sleep for 5 seconds */
    sleep(5);

    /* set LED trigger to heartbeat */
    status = mraa_led_set_trigger(led, LED_TRIGGER);
    if (status != MRAA_SUCCESS) {
        fprintf(stderr, "unable to set LED trigger to: heartbeat\n");
        goto err_exit;
    }

    fprintf(stdout, "LED trigger set to: heartbeat\n");

    /* close LED */
    mraa_led_close(led);

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
