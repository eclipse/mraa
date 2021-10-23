#!/usr/bin/env python

# Author: Costin Constantin <costin.c.constantin@intel.com>
# Copyright (c) 2015 Intel Corporation.
#
# SPDX-License-Identifier: MIT

from __future__ import print_function

import mraa as m
import unittest as u

@u.skipIf(m.getPlatformName() == "Unknown platform", "Not valid platform loaded")
class PlatformChecks(u.TestCase):
  pass


@u.skipUnless(m.getPlatformName() == "Intel Edison", "Only for edison Board")
class PlatformChecksEdison(u.TestCase):
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
    self.assertEqual(self.p_ADC_res, 10, "Wrong ADC supported resolution. Check platform ...")

if __name__ == "__main__":
  u.main()
