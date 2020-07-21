#!/usr/bin/env python

# Author: Brendan Le Foll <brendan.le.foll@intel.com>
# Copyright (c) 2014 Intel Corporation.
#
# SPDX-License-Identifier: MIT
#
# Example Usage: Reads integer and float value from ADC

import mraa

print(mraa.getVersion())

try:
    # initialise AIO
    x = mraa.Aio(0)

    # read integer value
    print(x.read())

    # read float value
    print("%.5f" % x.readFloat())
except:
    print("Are you sure you have an ADC?")
