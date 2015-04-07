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




#class test_GPIO():
 # def test_DIR_OUT(): 

def AIO_init():
  global a_pin
  try:
    a_pin = m.Aio(a_p)
    print("Initializing pin " + str(a_p) + " as AIO")
    return 0
  except ValueError:
    print(sys.exc_info()[1][0])
    return 1

class t_AIO(u.TestCase):
  def test_AIO_init(self):
    self.assertEqual(init_result,0, "AIO init failed on pin" + str(a_p))

  def test_get_AIO_resolution(self):
    self.no_of_bits = 8
    self.set_res = a_pin.setBit(self.no_of_bits)
    self.resolution = a_pin.getBit() #  i should have 8 bit resolution now 
    self.assertEqual(self.resolution , self.no_of_bits, "Cannot set resolution to desired " + str(self.no_of_bits) + " value")

  def test_set_AIO_resolution(self):
    self.no_of_bits = 5
    self.set_res = a_pin.setBit(self.no_of_bits)
    self.assertEqual(self.set_res, 0, "Cannot set resolution to desired " + str(self.set_res) + " value")

  def test_read_on_AIO(self):
    self.read_val = a_pin.read()
    self.assertLessEqual(self.read_val, 1000, "Wrong read measurement. Please check platform")


if __name__ == '__main__':
  a_p = 1
  a_pin = ''	
  init_result = AIO_init()
  if (init_result == 0):
    suite = u.TestLoader().loadTestsFromTestCase(t_AIO)
    u.main()

  else:
    print "Couldn't set the pin as AIO \n"


