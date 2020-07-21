#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

from uart_checks_shared import *

class UartChecksSetBaudrate(u.TestCase):
  def setUp(self):
    self.uart = m.Uart(MRAA_UART_DEV_NUM)

  def tearDown(self):
    del self.uart

  def test_uart_baudrate_115200(self):
    TEST_BAUDRATE = 115200
    self.assertEqual(self.uart.setBaudRate(TEST_BAUDRATE),
                     m.SUCCESS,
                     "Setting baudrate to %d did not return success" % TEST_BAUDRATE)

  def test_uart_baudrate_invalid_bigger_than_max(self):
    TEST_BAUDRATE = 10000000
    self.assertEqual(self.uart.setBaudRate(TEST_BAUDRATE),
                     m.ERROR_INVALID_PARAMETER,
                     "Setting baudrate to %d did not return INVALID_PARAMETER" % TEST_BAUDRATE)

if __name__ == "__main__":
  u.main()
