/*
 * Author: Brendan Le Foll
 * Contributors: Alex Tereschenko <alext.mkrs@gmail.com>
 * Contributors: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
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
 *
 * Example usage: Configures GPIO pin for interrupt and waits 30 seconds for the isr to trigger
 *
 */

/* standard headers */
#include <stdlib.h>
#include <unistd.h>

/* mraa header */
#include "mraa/gpio.h"

#define GPIO_PIN 6

void
int_handler(void* args)
{
    fprintf(stdout, "ISR triggered\n");
}

int
main()
{
    mraa_result_t status = MRAA_SUCCESS;
    mraa_gpio_context gpio;

    /* initialize mraa for the platform (not needed most of the times) */
    mraa_init();

    //! [Interesting]
    /* initialize GPIO pin */
    gpio = mraa_gpio_init(GPIO_PIN);
    if (gpio == NULL) {
        fprintf(stderr, "Failed to initialize GPIO %d\n", GPIO_PIN);
        mraa_deinit();
        return EXIT_FAILURE;
    }

    /* set GPIO to input */
    status = mraa_gpio_dir(gpio, MRAA_GPIO_IN);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }

    /* configure ISR for GPIO */
    status = mraa_gpio_isr(gpio, MRAA_GPIO_EDGE_BOTH, &int_handler, NULL);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }

    /* wait 30 seconds isr trigger */
    sleep(30);

    /* close GPIO */
    mraa_gpio_close(gpio);

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
