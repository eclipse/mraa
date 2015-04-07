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

def I2C_init():
  global I2C_port
  try:
    I2C_port = m.I2c(i2c_p)
    print("Initializing port " + str(i2c_p) + " as I2C")
    return 0
  except ValueError:
    print(sys.exc_info()[1][0])
    return 1

class t_I2C(u.TestCase): # will test on HTU21 sensor. In the near future, tests will be made on a variety of devices
  def test_I2C_init(self):
    self.assertEqual(init_result,0, "I2C init failed on port " + str(i2c_p))

  def test_I2C_set_address(self):
    self.address = I2C_port.address(0x40) # setting a corect 7 bit address
    self.assertEqual(self.address, 0, "Cannot set I2C address. Check board.")

  def test_I2C_set_wrong_address(self):                                                  
    self.address = I2C_port.address(0x80) # setting wrong 7 bit address. This is the first address after 127
    self.assertNotEqual(self.address, 0, "Wrong address set. This is not possible for 7 bit addresses.") 

  def test_I2C_write_byte(self):
    I2C_port.address(0x40) # sending a config byte on the bus. This is for HTU21
    self.w_byte = I2C_port.writeByte(0xF3)
    self.assertEqual(self.w_byte, 0, "Cannot write byte on the bus. Check board.")

  def test_I2C_write_wrong_byte(self):                                                                                                                                    
    I2C_port.address(0x40) # sending a wrong config byte on the bus. This is for HTU21                                                                                    
    self.w_byte = I2C_port.writeByte(0xF0)                                                                                                                               
    self.assertNotEqual(self.w_byte, 0, "Wrong byte sent somewhere. This is not good. Please check board.") 
                       
  def test_I2C_read_byte(self):                                                                                                                                    
    I2C_port.address(0x40) # sending a config byte on the bus. This is for HTU21                                                                                    
    I2C_port.writeByte(0xF3)
    sleep(0.1)
    self.raw = bytearray(3)     
    self.raw = I2C_port.read(3)
    self.data_bin = bin(self.raw[0])[2:] + bin(self.raw[1])[2:-2] 
    self.data_int = int(self.data_bin,2)  
    self.temp = 175.72 * self.data_int/65536 - 46.85
    self.assertGreater(self.temp, 15, "Too cold don't you think? ..." + str(self.temp) + " deg. C") 

  def test_I2C_write_reg(self):     
    I2C_port.address(0x40) # sending a config byte on the bus. This is for HTU21
    sleep(0.1)
    self.w_reg = I2C_port.writeReg(0xE6, 0x62)
    self.assertEqual(self.w_reg, 0, "Failed to write user register ...")

  def test_I2C_write_reg(self):                                                   
    I2C_port.address(0x40) # sending a config byte on the bus. This is for HTU21  
    sleep(0.1)                                                                    
    I2C_port.writeReg(0xE6, 0x33)   
    self.assertEqual(int(I2C_port.readReg(0xE7)), 51, "Failed to write user register ...") 



if __name__ == '__main__':
  i2c_p = 0
  I2C_port = ''	
  init_result = I2C_init()
  if (init_result == 0):
    suite = u.TestLoader().loadTestsFromTestCase(t_I2C)
    u.main()

  else:
    print "Couldn't set port " + str(i2c_p) + "as I2C"


