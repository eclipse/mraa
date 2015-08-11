#!/usr/bin/env python

# Author: Costin Constantin <costin.c.constantin@intel.com>
# Copyright (c) 2015 Intel Corporation.
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

from __future__ import print_function

import mraa as m
import unittest as u

@u.skipIf(m.init() != m.ERROR_PLATFORM_ALREADY_INITIALISED, "mraa_init() not valid")
class PlatformChecks(u.TestCase):
  def test_mraa_check_platform_no_of_pins(self):
    pinCount = m.getPinCount()
    self.assertEqual(pinCount, 20, "Wrong number. of pins. Check platform ...")

  def test_mraa_check_platform_ADC_max_resolution(self):
    self.p_ADC_mres = m.adcRawBits()
    print("Platform ADC max. resolution is: " + str(self.p_ADC_mres) + " bits")
    self.assertEqual(self.p_ADC_mres, 12, "Wrong ADC max. resolution. Check platform ...")

  def test_mraa_check_platform_ADC_resolution(self):
    self.p_ADC_res = m.adcSupportedBits()
    print("Platform ADC resolution is: " + str(self.p_ADC_res) + " bits")
    self.assertEqual(self.p_ADC_res, 10, "Wrong ADC suported resolution. Check platform ...")

if __name__ == "__main__":
  u.main()
