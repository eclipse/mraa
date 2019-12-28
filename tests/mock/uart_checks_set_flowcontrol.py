#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

from uart_checks_shared import *

class UartChecksSetFlowControl(u.TestCase):
  def setUp(self):
    self.uart = m.Uart(MRAA_UART_DEV_NUM)

  def tearDown(self):
    del self.uart

  def test_uart_set_flowcontrol(self):
    self.assertEqual(self.uart.setFlowcontrol(False, True),
                     m.SUCCESS,
                     "Running UART setFlowControl() did not return success")

if __name__ == "__main__":
  u.main()
