#!/usr/bin/env python

# Author: Jon Trulson <jtrulson@ics.com>
# Copyright (c) 2017 Intel Corporation.
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

from uart_checks_shared import *

class UartChecksSendbreak(u.TestCase):
  def setUp(self):
    self.uart = m.Uart(MRAA_UART_DEV_NUM)

  def tearDown(self):
    del self.uart

  def test_uart_sendbreak(self):
    self.assertEqual(self.uart.sendBreak(0),
                     m.SUCCESS,
                     "Running UART sendBreak() did not return success")

if __name__ == "__main__":
  u.main()
