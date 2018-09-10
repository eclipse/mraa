/*
 * Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 * Copyright (c) 2018, Linaro Ltd.
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
