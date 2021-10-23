/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 * Example usage: Generates PWM at a step rate of 0.01 continuously.
 *                Press Ctrl+C to exit
 */

/* standard headers */
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

/* mraa headers */
#include "mraa/common.hpp"
#include "mraa/pwm.hpp"

#define PWM_PORT 3

volatile sig_atomic_t flag = 1;

void
sig_handler(int signum)
{
    if (signum == SIGINT) {
        std::cout << "Exiting..." << std::endl;
        flag = 0;
    }
}

int
main(void)
{
    float value = 0.0f;

    signal(SIGINT, sig_handler);

    //! [Interesting]
    mraa::Pwm pwm(PWM_PORT);
    std::cout << "Cycling PWM on IO3 (pwm3)" << std::endl;
    pwm.enable(true);

    while (flag) {
        value = value + 0.01f;
        pwm.write(value);
        usleep(50000);
        if (value >= 1.0f) {
            value = 0.0f;
        }
    }
    //! [Interesting]

    return EXIT_SUCCESS;
}
