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
import sys
import unittest as u
import os, re
from time import sleep

def UART_init():
  global u_port
  try:
    u_port = m.Uart(u_p)
    print("Initializing pin " + str(u_p) + " as UART")
    return 0
  except ValueError:
    print(sys.exc_info()[1][0])
    return 1

class t_UART(u.TestCase):
  def test_UART_init(self):
    self.assertEqual(init_result,0, "UART init failed on port " + str(u_p))

  def test_UART_get_device_path(self):
    self.dev_path = u_port.getDevicePath()
    self.assertRegexpMatches(self.dev_path, "/dev/tty*")


if __name__ == '__main__':
  u_p = 0
  u_port = ''	
  init_result = UART_init()
  if (init_result == 0):
    suite = u.TestLoader().loadTestsFromTestCase(t_UART)
    u.main()

  else:
    print "Couldn't set UART \n"


