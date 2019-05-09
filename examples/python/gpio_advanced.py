#!/usr/bin/env python

# Author: Brendan Le Foll <brendan.le.foll@intel.com>
# Copyright (c) 2014 Intel Corporation.
#
# SPDX-License-Identifier: MIT
#
# Example Usage: Triggers ISR upon GPIO state change

import mraa
import time
import sys

class Counter:
    count = 0

c = Counter()

# inside a python interrupt you cannot use 'basic' types so you'll need to use
# objects
def isr_routine(gpio):
    print("pin " + repr(gpio.getPin(True)) + " = " + repr(gpio.read()))
    c.count += 1

# GPIO
pin = 6;

try:
    # initialise GPIO
    x = mraa.Gpio(pin)

    print("Starting ISR for pin " + repr(pin))

    # set direction and edge types for interrupt
    x.dir(mraa.DIR_IN)
    x.isr(mraa.EDGE_BOTH, isr_routine, x)

    # wait until ENTER is pressed
    var = raw_input("Press ENTER to stop")
    x.isrExit()
except ValueError as e:
    print(e)
