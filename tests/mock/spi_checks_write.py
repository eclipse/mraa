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

from spi_checks_shared import *

class SpiChecksWrite(u.TestCase):
  def setUp(self):
    self.spi = m.Spi(MRAA_SPI_BUS_NUM)

  def tearDown(self):
    del self.spi

  def test_spi_write(self):
    DATA_TO_WRITE = bytearray([0xEE for i in range(MOCK_SPI_TEST_DATA_LEN)])
    DATA_TO_EXPECT = bytearray([0xEE ^ MOCK_SPI_REPLY_DATA_MODIFIER_BYTE for i in range(MOCK_SPI_TEST_DATA_LEN)])
    self.assertEqual(self.spi.write(DATA_TO_WRITE),
                     DATA_TO_EXPECT,
                     "SPI write() returned unexpected data")

if __name__ == "__main__":
  u.main()
