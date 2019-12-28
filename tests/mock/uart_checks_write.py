#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

from uart_checks_shared import *

class UartChecksWrite(u.TestCase):
  def setUp(self):
    self.uart = m.Uart(MRAA_UART_DEV_NUM)

  def tearDown(self):
    del self.uart

  def test_uart_write(self):
    TEST_DATA_LEN = 10
    TEST_DATA = bytearray([x for x in range(TEST_DATA_LEN)])
    self.assertEqual(self.uart.write(TEST_DATA),
                     TEST_DATA_LEN,
                     "Running UART write(%s) did not return %d" % (repr(TEST_DATA), TEST_DATA_LEN))

  def test_uart_writeStr(self):
    TEST_DATA = "Hello"
    TEST_DATA_LEN = len(TEST_DATA)
    self.assertEqual(self.uart.writeStr(TEST_DATA),
                     TEST_DATA_LEN,
                     "Running UART writeStr(%s) did not return %d" % (TEST_DATA, TEST_DATA_LEN))

if __name__ == "__main__":
  u.main()
