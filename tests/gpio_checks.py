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
import os, re, sys
from time import sleep

MRAA_GPIO = 3

@u.skipIf(m.pinModeTest(MRAA_GPIO, m.PIN_GPIO) != True, str(MRAA_GPIO) + "is not a valid Gpio on this platform")
class GpioChecks(u.TestCase):

  def setUp(self):
    self.pin = m.Gpio(MRAA_GPIO)
    self.gpio_path = "/sys/class/gpio/gpio" + str(self.pin.getPin(True))

  def tearDown(self):
    # dereference pin to force cleanup
    self.pin = ""

  def test_returning_pin_no(self):
    self.pin_no = self.pin.getPin() # i should have the pin no. here as set when initing Gpio class
    self.assertEqual(self.pin_no, MRAA_GPIO, "Something wrong happened ... set pin doesn't correspond to retrieved one")

  def test_returning_pin_as_on_sys(self):
    self.assertTrue(os.path.exists(self.gpio_path))

  def test_set_GPIO_as_output(self):
    self.pin.dir(m.DIR_OUT)
    dir_file = open(self.gpio_path + "/direction")
    dir_file_content = dir_file.readline()[:-1]
    dir_file.close()
    self.assertMultiLineEqual(dir_file_content, "out")

  def test_set_GPIO_as_input(self):
    self.pin.dir(m.DIR_IN)
    dir_file = open(self.gpio_path + "/direction")
    dir_file_content = dir_file.readline()[:-1]
    dir_file.close()
    self.assertMultiLineEqual(dir_file_content, "in")

  def test_GPIO_as_output_write_HIGH_level(self):
    self.pin.dir(m.DIR_OUT)
    self.pin.write(1)
    val_file = open(self.gpio_path + "/value")
    sysfs_pin_value = val_file.readline()[:-1]
    val_file.close()
    self.assertEqual(int(sysfs_pin_value),1, "Value doesn't match the HIGH state")

  def test_GPIO_as_output_write_LOW_level(self):
    self.pin.dir(m.DIR_OUT)
    self.pin.write(0)
    val_file = open(self.gpio_path + "/value")
    sysfs_pin_value = val_file.readline()[:-1]
    val_file.close()
    self.assertEqual(int(sysfs_pin_value), 0, "Value doesn't match the LOW state")

  def test_GPIO_as_input_and_write_HIGH_on_it(self):
    self.pin.dir(m.DIR_IN)
    res = self.pin.write(1)
    self.assertNotEqual(res, m.SUCCESS, "The command should fail")

  def test_GPIO_as_input_and_write_LOW_on_it(self):
    self.pin.dir(m.DIR_IN)
    res = self.pin.write(0)
    self.assertGreater(res, 0, "The command should have returned value greater than 0")

if __name__ == '__main__':
  u.main()
