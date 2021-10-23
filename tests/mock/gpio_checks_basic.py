#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko <alext.mkrs@gmail.com>
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

MRAA_TEST_PIN = 0

class GpioChecksBasic(u.TestCase):
  def setUp(self):
    self.pin = m.Gpio(MRAA_TEST_PIN)

  def tearDown(self):
    del self.pin

  def test_gpio_state_after_init(self):
      # After GPIO init it should be in INPUT and LOW state
      self.assertEqual(self.pin.read(), 0, "GPIO is in a wrong state after init")

  def test_gpio_dir_after_init(self):
      # After GPIO init it should be in INPUT and LOW state
      self.assertEqual(self.pin.readDir(), m.DIR_IN, "GPIO has wrong direction after init")

  def test_get_pin_num(self):
    self.assertEqual(self.pin.getPin(), MRAA_TEST_PIN, "Returned GPIO pin number is incorrect")

if __name__ == '__main__':
  u.main()
