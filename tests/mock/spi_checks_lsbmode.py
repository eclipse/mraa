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

class SpiChecksLsbmode(u.TestCase):
  def setUp(self):
    self.spi = m.Spi(MRAA_SPI_BUS_NUM)

  def tearDown(self):
    del self.spi

  def test_spi_set_lsbmode_false(self):
    TEST_LSBMODE = False
    self.assertEqual(self.spi.lsbmode(TEST_LSBMODE),
                     m.SUCCESS,
                     "Setting LSB mode to %s did not return success" %TEST_LSBMODE)

  def test_spi_set_lsbmode_true(self):
    TEST_LSBMODE = True
    self.assertEqual(self.spi.lsbmode(TEST_LSBMODE),
                     m.SUCCESS,
                     "Setting LSB mode to %s did not return success" %TEST_LSBMODE)

  def test_spi_set_lsbmode_invalid(self):
    TEST_LSBMODE = 10
    self.assertRaises(TypeError, self.spi.lsbmode, TEST_LSBMODE)


if __name__ == "__main__":
  u.main()
