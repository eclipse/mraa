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

from i2c_checks_shared import *

class I2cChecksFreq(u.TestCase):
  def setUp(self):
    self.i2c = m.I2c(MRAA_I2C_BUS_NUM)

  def tearDown(self):
    del self.i2c

  def test_i2c_frequency_STD(self):
    self.assertEqual(self.i2c.frequency(m.I2C_STD),
                     m.SUCCESS,
                     "Setting frequency to I2C_STD did not return success")

  def test_i2c_frequency_FAST(self):
    self.assertEqual(self.i2c.frequency(m.I2C_FAST),
                     m.SUCCESS,
                     "Setting frequency to I2C_FAST did not return success")

  def test_i2c_frequency_HIGH(self):
    self.assertEqual(self.i2c.frequency(m.I2C_HIGH),
                     m.SUCCESS,
                     "Setting frequency to I2C_HIGH did not return success")

  def test_i2c_frequency_invalid_bigger_than_max(self):
    self.assertEqual(self.i2c.frequency(100),
                     m.ERROR_INVALID_PARAMETER,
                     "Setting frequency to 100 did not return INVALID_PARAMETER")

  def test_i2c_frequency_invalid_smaller_than_min(self):
    self.assertEqual(self.i2c.frequency(-100),
                     m.ERROR_INVALID_PARAMETER,
                     "Setting frequency to -100 did not return INVALID_PARAMETER")

if __name__ == "__main__":
  u.main()
