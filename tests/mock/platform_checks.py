#!/usr/bin/env python

# Author: Costin Constantin <costin.c.constantin@intel.com>
# Copyright (c) 2015 Intel Corporation.
#
# Contributors: Alex Tereschenko <alext.mkrs@gmail.com>
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

PLATFORM_PINCOUNT = 10
PLATFORM_STD_ADC_RES_BITS = 10
PLATFORM_MAX_ADC_RES_BITS = 12

class PlatformChecks(u.TestCase):
  def test_platform_pin_count(self):
    self.assertEqual(m.getPinCount(), PLATFORM_PINCOUNT, "Wrong number of pins reported by platform")

  def test_adc_std_res(self):
    adc_std_res = m.adcSupportedBits()
    print("Platform ADC standard resolution is: " + str(adc_std_res) + " bits")
    self.assertEqual(adc_std_res, PLATFORM_STD_ADC_RES_BITS, "Wrong ADC standard resolution")

  def test_adc_max_res(self):
    adc_max_res = m.adcRawBits()
    print("Platform ADC max. resolution is: " + str(adc_max_res) + " bits")
    self.assertEqual(adc_max_res, PLATFORM_MAX_ADC_RES_BITS, "Wrong ADC max. resolution")

if __name__ == "__main__":
  u.main()
