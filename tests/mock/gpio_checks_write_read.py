#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko <alext.mkrs@gmail.com>
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

MRAA_TEST_PIN = 0

class GpioChecksWriteRead(u.TestCase):
  def setUp(self):
    self.pin = m.Gpio(MRAA_TEST_PIN)

  def tearDown(self):
    del self.pin

  def test_gpio_as_output_write_HIGH(self):
    self.pin.dir(m.DIR_OUT)
    res = self.pin.write(1)
    self.assertEqual(res, m.SUCCESS, "Setting GPIO to HIGH failed")
    res = self.pin.read()
    self.assertEqual(res, 1, "GPIO is not HIGH after write(1)")

  def test_gpio_as_output_write_LOW(self):
    self.pin.dir(m.DIR_OUT)
    res = self.pin.write(0)
    self.assertEqual(res, m.SUCCESS, "Setting GPIO to LOW failed")
    res = self.pin.read()
    self.assertEqual(res, 0, "GPIO is not LOW after write(0)")

  def test_gpio_as_output_write_invalid(self):
    self.pin.dir(m.DIR_OUT)
    value = 10
    res = self.pin.write(value)
    self.assertNotEqual(res, m.SUCCESS, "Writing " + str(value) + " to GPIO should have failed")
    res = self.pin.read()
    self.assertNotEqual(res, value, "Writing " + str(value) + " to GPIO should not have set it to " + str(value))

  def test_gpio_as_input_write_HIGH(self):
    self.pin.dir(m.DIR_IN)
    res = self.pin.write(1)
    self.assertNotEqual(res, m.SUCCESS, "Setting GPIO in INPUT to HIGH should have failed")

  def test_gpio_as_input_write_LOW(self):
    self.pin.dir(m.DIR_IN)
    res = self.pin.write(0)
    self.assertNotEqual(res, m.SUCCESS, "Setting GPIO in INPUT to LOW should have failed")

if __name__ == '__main__':
  u.main()
