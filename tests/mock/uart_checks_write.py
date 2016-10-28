#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

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
