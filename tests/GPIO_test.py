#!/usr/bin/env python

# Author: Costin Constantin <costin.c.constantin@intel.com>
# Copyright (c) 2014 Intel Corporation.
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
import os, re
from time import sleep

def GPIO_init():
  global pin
  try:
    pin = m.Gpio(p)
    print("Initializing pin " + str(p) + " as GPIO")
    return 0
  except ValueError:
    print(sys.exc_info()[1][0])
    return 1

class t_GPIO(u.TestCase):
  def test_GPIO_init(self):
    self.assertEqual(init_result,0, "nope")

  def test_returning_pin_no(self):
    self.pin_no = pin.getPin() # i should have the pin no. here as set when initing Gpio class 
    global p
    self.assertEqual(self.pin_no, p, "Something wrong happened ... set pin doesn't correspond to retrieved one")

  def test_returning_pin_as_on_sys(self):
    self.gpio_path = "/sys/class/gpio/gpio" + str(pin.getPin(True))
    self.assertTrue(os.path.exists(self.gpio_path))

  def test_set_GPIO_as_output(self):
    self.gpio_path = "/sys/class/gpio/gpio" + str(pin.getPin(True))   
    pin.dir(m.DIR_OUT)
    self.dir_file = open(self.gpio_path + "/direction")
    self.dir_file_content = self.dir_file.readline()[:-1]
    self.dir_file.close()    
    self.assertMultiLineEqual(self.dir_file_content, "out")     

  def test_set_GPIO_as_input(self):
    self.gpio_path = "/sys/class/gpio/gpio" + str(pin.getPin(True))
    pin.dir(m.DIR_IN)
    self.dir_file = open(self.gpio_path + "/direction")
    self.dir_file_content = self.dir_file.readline()[:-1]
    self.dir_file.close()
    self.assertMultiLineEqual(self.dir_file_content, "in")

  def test_GPIO_as_output_write_HIGH_level(self):
    self.gpio_path = "/sys/class/gpio/gpio" + str(pin.getPin(True))                                             
    pin.dir(m.DIR_OUT)
    pin.write(1)
    self.val_file = open(self.gpio_path + "/value")            
    self.val_file_content = self.val_file.readline()[:-1]          
    self.val_file.close()
    self.assertEqual(int(self.val_file_content),1, "Value doesn't match the HIGH state")

  def test_GPIO_as_output_write_LOW_level(self):                  
    self.gpio_path = "/sys/class/gpio/gpio" + str(pin.getPin(True))
    pin.dir(m.DIR_OUT)                                             
    pin.write(0)                                                   
    self.val_file = open(self.gpio_path + "/value")                
    self.val_file_content = self.val_file.readline()[:-1]          
    self.val_file.close()                                          
    self.assertEqual(int(self.val_file_content), 0, "Value doesn't match the LOW state") 
    
  def test_GPIO_as_input_and_write_HIGH_on_it(self):
    self.gpio_path = "/sys/class/gpio/gpio" + str(pin.getPin(True))                                             
    pin.dir(m.DIR_IN)   
    self.rezult_of_command = pin.write(1)                                                                                         
    self.assertGreater(self.rezult_of_command, 0, "The command should have returned value bigger than 0")     

  def test_GPIO_as_input_and_write_LOW_on_it(self):                                                                
    self.gpio_path = "/sys/class/gpio/gpio" + str(pin.getPin(True))                                             
    pin.dir(m.DIR_IN)                                                                                           
    self.rezult_of_command = pin.write(0)                                                                         
    self.assertGreater(self.rezult_of_command, 0, "The command should have returned value greater than 0")



if __name__ == '__main__':
  p = 3
  pin = ''	
  init_result = GPIO_init()
  if (init_result == 0):
    suite = u.TestLoader().loadTestsFromTestCase(t_GPIO)
    u.main()

    sleep(10)
  else:
    print "Couldn't set the pin as GPIO \n"
  set_dir()


