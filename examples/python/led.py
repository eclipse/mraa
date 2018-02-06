#!/usr/bin/env python

# Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
# Copyright (c) 2018 Linaro Ltd.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
