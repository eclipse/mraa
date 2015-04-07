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
from time import sleep

def raw_temp(t):
  # according to TMP125 datasheet, between 10 and 50 deg C, one of the bits 13,12 and 11 should be of value 1
  mask = 14336 # this is the mask to isolate bits 13,12,11 ---> '0011100000000000' 
  read_val = t
  return (read_val & mask)


def SPI_init():
  global SPI_port
  try:
    SPI_port = m.Spi(spi_p)
    print("Initializing port " + str(spi_p) + " as SPI")
    return 0
  except ValueError:
    print(sys.exc_info()[1][0])
    return 1

class t_SPI(u.TestCase): # will test on TMP125 sensor.
  def test_SPI_init(self):
    self.assertEqual(init_result,0, "SPI init failed on port " + str(spi_p))

  def test_set_frequency(self):
    self.f_value_in_Hz = 100000
    self.res = SPI_port.frequency(self.f_value_in_Hz)
    self.assertEqual(self.res, 0, "Cannot set frequency on SPI")
   
  def test_set_lsbmode_MSB_first(self):  # with lsbmode(), the way bits come out on the SO pin of the sensor is set up 
    self.res = SPI_port.lsbmode(False) # like this, the first is MSB
    self.assertEqual(self.res, 0, "Cannot set MSB first. Maybe your platform doesn't support it")

  def test_set_lsbmode_LSB_first(self):  # with lsbmode(), the way bits come out on the SO pin of the sensor is set up
    self.res = SPI_port.lsbmode(True) # like this LSB is first
    self.assertEqual(self.res, 0, "Cannot set LSB first ...")

  def test_set_bits_per_word(self):
    self.no_of_bits = 32
    self.res = SPI_port.bitPerWord(self.no_of_bits)
    self.assertEqual(self.res, 0, "Cannot set no of bits")

  def test_write_word(self): # this will use the software CS, that is pin 10 on Galileo v1
    self.no_of_bits = 16
    SPI_port.bitPerWord(self.no_of_bits)
    self.res = SPI_port.write_word(0) # sending something on the bus. It does not matter however because the TMP125 sensor has only one read-only reg.
    self.r_t = raw_temp(int(self.res))
    # for the given sensor TMP125 the four bits returned should be 0001 for a temperature less than 50 deg.    
    self.assertGreater(self.r_t, 0, "Sensor value out of range")
    
  def test_write_byte(self): # this will use the software CS, that is pin 10 on Galileo v1
    self.no_of_bits = 8                                                          
    SPI_port.bitPerWord(self.no_of_bits)
    sleep(0.2)                                          
    self.res_MSB = SPI_port.writeByte(0) # sending something on the bus. 
    self.res_LSB = SPI_port.writeByte(0) # again what it is sent on the bus does not matter
    # for the given sensor TMP125 the four bits returned should be 0001 for a temp
    self.res_bin_MSB = "0"*(8 - len(bin(self.res_MSB)[2:])) + bin(self.res_MSB)[2:]   
    self.res_bin_LSB = "0"*(8 - len(bin(self.res_LSB)[2:])) + bin(self.res_LSB)[2:]
    self.r_t = raw_temp(int(self.res_bin_MSB + self.res_bin_LSB))
    self.assertGreater(self.r_t, 0, "Sensor value out of range")  
                                                                                
  def test_write(self): 
    self.no_of_bits = 8                                                                                                                                             
    SPI_port.bitPerWord(self.no_of_bits)                                                                                                                            
    sleep(0.2)  # here will have to use a bytearray                                                                                                                                                    
    self.res = bytearray(2)
    self.var = bytearray(2)
    self.res = SPI_port.write(self.var) # sending something on the bus.   
    self.res_0 = "0"*(8 - len(bin(self.res[0])[2:])) + bin(self.res[0])[2:]                                                                                         
    self.res_1 = "0"*(8 - len(bin(self.res[1])[2:])) + bin(self.res[1])[2:]
    self.r_t = raw_temp(int(self.res_0 + self.res_1))                
    self.assertGreater(self.r_t, 0, "Sensor value out of range")

if __name__ == '__main__':
  spi_p = 0
  SPI_port = ''	
  init_result = SPI_init()
  if (init_result == 0):
    suite = u.TestLoader().loadTestsFromTestCase(t_SPI)
    u.main()

  else:
    print "Couldn't set port " + str(spi_p) + "as SPI"


