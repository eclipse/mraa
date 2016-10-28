#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko <alext.mkrs@gmail.com>
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
