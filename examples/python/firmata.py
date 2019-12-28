#!/usr/bin/env python

# Author: Ron Evans (@deadprogram)
# Copyright (c) 2016 Intel Corporation.
#
# SPDX-License-Identifier: MIT

import mraa

print (mraa.getVersion())

mraa.addSubplatform(mraa.GENERIC_FIRMATA, "/dev/ttyACM0");
