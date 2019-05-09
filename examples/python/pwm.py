#!/usr/bin/env python

# Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
# Copyright (c) 2014 Intel Corporation.
#
# SPDX-License-Identifier: MIT
#
# Example Usage: Generates PWM at a step rate of 0.01 continuously.

import mraa
import time

# initialise PWM
x = mraa.Pwm(3)

# set PWM period
x.period_us(700)

# enable PWM
x.enable(True)

value= 0.0

while True:
    # write PWM value
    x.write(value)

    time.sleep(0.05)

    value = value + 0.01
    if value >= 1:
        value = 0.0
