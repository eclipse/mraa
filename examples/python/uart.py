#!/usr/bin/env python

# Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
# Copyright (c) 2018 Linaro Ltd.
#
# SPDX-License-Identifier: MIT
#
# Example Usage: Sends data through UART

import mraa
import time
import sys

# serial port
port = "/dev/ttyACM0"

data = 'Hello Mraa!'

# initialise UART
uart = mraa.Uart(port)

# send data through UART
uart.write(bytearray(data, 'utf-8'))
