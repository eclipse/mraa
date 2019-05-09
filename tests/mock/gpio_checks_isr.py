#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko <alext.mkrs@gmail.com>
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

MRAA_TEST_PIN = 0

def test_isr():
    print("In the ISR")

class GpioChecksIsr(u.TestCase):
  def setUp(self):
    self.pin = m.Gpio(MRAA_TEST_PIN)

  def tearDown(self):
    del self.pin

  def test_set_isr(self):
      self.pin.dir(m.DIR_IN)
      res = self.pin.isr(m.EDGE_BOTH, test_isr, None)
      self.assertEqual(res, m.ERROR_FEATURE_NOT_IMPLEMENTED, "Enabling ISR did not return unimplemented")

  def test_isr_exit(self):
      self.pin.dir(m.DIR_IN)
      self.pin.isr(m.EDGE_BOTH, test_isr, None)
      res = self.pin.isrExit()
      self.assertEqual(res, m.ERROR_FEATURE_NOT_IMPLEMENTED, "Calling isrExit() did not return unimplemented")

if __name__ == '__main__':
  u.main()
