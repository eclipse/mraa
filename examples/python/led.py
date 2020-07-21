#!/usr/bin/env python

# Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
# Copyright (c) 2018 Linaro Ltd.
#
# SPDX-License-Identifier: MIT
#
# Example Usage: Reads maximum brightness value for user1 led and turns it
#                on/off depending on current state. Then sets led trigger
#                to heartbeat

import mraa
import time

# initialise user1 led
led_1 = mraa.Led("user1")

# read maximum brightness
val = led_1.readMaxBrightness()

print("maximum brightness value for user1 led is: %d" % val);

# turn led on/off depending on read max_brightness value
if (val >= 1):
    val = 0
# never reached mostly
else:
    val = 1

# set LED brightness
led_1.setBrightness(val)

# sleep for 5 seconds
time.sleep(5)

led_1.trigger("heartbeat")

print("led trigger set to: heartbeat")
