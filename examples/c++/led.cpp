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
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

/* mraa headers */
#include "mraa/common.hpp"
#include "mraa/led.hpp"

/* LED name */
#define USER_LED "user1"

/* trigger type */
#define LED_TRIGGER "heartbeat"

int
main(void)
{
    mraa::Result status;
    int val;

    //! [Interesting]
    /* initialize user1 led */
    mraa::Led led(USER_LED);

    /* read maximum brightness */
    val = led.readMaxBrightness();
    std::cout << "maximum brightness value for user1 led is: %d" << val << std::endl;
    if (val >= 1) {
        val = 0;
    } else {
        /* never reached mostly */
        val = 1;
    }

    /* turn led on/off depending on max_brightness value */
    status = led.setBrightness(val);
    if (status != mraa::SUCCESS) {
        printError(status);
        return EXIT_FAILURE;
    }

    /* sleep for 5 seconds */
    usleep(5000000);

    /* set led trigger to heartbeat */
    status = led.trigger(LED_TRIGGER);
    if (status != mraa::SUCCESS) {
        printError(status);
        return EXIT_FAILURE;
    }

    std::cout << "led trigger set to: heartbeat" << std::endl;
    //! [Interesting]

    return EXIT_SUCCESS;
}
