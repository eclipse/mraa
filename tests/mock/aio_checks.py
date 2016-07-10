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

MRAA_AIO_TEST_PIN = 0
PLATFORM_STD_ADC_RES_BITS = 10
PLATFORM_MAX_ADC_RES_BITS = 12

class AioChecks(u.TestCase):
  def setUp(self):
    self.pin = m.Aio(MRAA_AIO_TEST_PIN)

  def tearDown(self):
    del self.pin

  def test_aio_get_bit(self):
    self.assertEqual(self.pin.getBit(), PLATFORM_STD_ADC_RES_BITS, "Wrong ADC resolution reported")

  def test_aio_set_bit(self):
    self.pin.setBit(PLATFORM_MAX_ADC_RES_BITS)
    self.assertEqual(self.pin.getBit(), PLATFORM_MAX_ADC_RES_BITS, "Wrong ADC resolution reported after setBit()")

  def test_aio_read(self):
    self.assertNotEqual(self.pin.read(), -1, "Error returned when reading ADC value")

  def test_aio_read_float_std_res(self):
    self.pin.setBit(PLATFORM_STD_ADC_RES_BITS)
    self.assertNotEqual(self.pin.readFloat(), -1, "Error returned when reading float ADC value at standard resolution")

  def test_aio_read_float_max_res(self):
    self.pin.setBit(PLATFORM_MAX_ADC_RES_BITS)
    self.assertNotEqual(self.pin.readFloat(), -1, "Error returned when reading float ADC value at maximum resolution")

if __name__ == "__main__":
  u.main()
