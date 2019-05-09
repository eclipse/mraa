#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

from uart_checks_shared import *

class UartChecksRead(u.TestCase):
  def setUp(self):
    self.uart = m.Uart(MRAA_UART_DEV_NUM)

  def tearDown(self):
    del self.uart

  def test_uart_read(self):
    TEST_DATA_LEN = 10
    EXPECTED_RESULT = bytearray([MOCK_UART_DATA_BYTE for x in range(TEST_DATA_LEN)])
    self.assertEqual(self.uart.read(TEST_DATA_LEN),
                     EXPECTED_RESULT,
                     "Running UART read(%d) did not return %s" % (TEST_DATA_LEN, repr(EXPECTED_RESULT)))

  def test_uart_readStr(self):
    TEST_DATA_LEN = 10
    EXPECTED_RESULT = chr(MOCK_UART_DATA_BYTE) * TEST_DATA_LEN
    self.assertEqual(self.uart.readStr(TEST_DATA_LEN),
                     EXPECTED_RESULT,
                     "Running UART readStr(%d) did not return %s" % (TEST_DATA_LEN, EXPECTED_RESULT))

if __name__ == "__main__":
  u.main()
