#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

from uart_checks_shared import *

class UartChecksSetMode(u.TestCase):
  def setUp(self):
    self.uart = m.Uart(MRAA_UART_DEV_NUM)

  def tearDown(self):
    del self.uart

  def test_uart_set_mode(self):
    self.assertEqual(self.uart.setMode(8, m.UART_PARITY_NONE, 1),
                     m.SUCCESS,
                     "Running UART setMode() did not return success")

if __name__ == "__main__":
  u.main()
