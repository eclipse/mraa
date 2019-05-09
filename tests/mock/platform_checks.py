#!/usr/bin/env python

# Author: Costin Constantin <costin.c.constantin@intel.com>
# Copyright (c) 2015 Intel Corporation.
#
# Contributors: Alex Tereschenko <alext.mkrs@gmail.com>
#
# SPDX-License-Identifier: MIT

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
