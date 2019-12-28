#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

from uart_checks_shared import *

class UartChecksDataAvailable(u.TestCase):
  def setUp(self):
    self.uart = m.Uart(MRAA_UART_DEV_NUM)

  def tearDown(self):
    del self.uart

  def test_uart_data_available(self):
    self.assertEqual(self.uart.dataAvailable(10),
                     True,
                     "Running UART dataAvailable() did not return True")

if __name__ == "__main__":
  u.main()
