/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Contributors: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 * Example usage: Generates PWM signal of period 200us with variable dutycyle
 *                repeately. Press Ctrl+C to exit
 */

/* standard headers */
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

/* mraa header */
#include "mraa/pwm.h"

/* PWM declaration */
#define PWM 3

/* PWM period in us */
#define PWM_FREQ 200

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
    mraa_pwm_context pwm;
    float value = 0.0f;
    float output;

    /* initialize mraa for the platform (not needed most of the times) */
    mraa_init();

    //! [Interesting]
    pwm = mraa_pwm_init(PWM);
    if (pwm == NULL) {
        fprintf(stderr, "Failed to initialize PWM\n");
        mraa_deinit();
        return EXIT_FAILURE;
    }

    /* set PWM period */
    status = mraa_pwm_period_us(pwm, PWM_FREQ);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }

    /* enable PWM */
    status = mraa_pwm_enable(pwm, 1);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }

    while (flag) {
        value = value + 0.01f;

        /* write PWM duty cyle */
        status = mraa_pwm_write(pwm, value);
        if (status != MRAA_SUCCESS) {
            goto err_exit;
        }

        usleep(50000);

        if (value >= 1.0f) {
            value = 0.0f;
        }

        /* read PWM duty cyle */
        output = mraa_pwm_read(pwm);
        fprintf(stdout, "PWM value is %f\n", output);
    }

    /* close PWM */
    mraa_pwm_close(pwm);

    //! [Interesting]
    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_SUCCESS;

err_exit:
    mraa_result_print(status);

    /* close PWM */
    mraa_pwm_close(pwm);

    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_FAILURE;
}
