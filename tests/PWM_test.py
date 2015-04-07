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
import glob as g
import commands as c


def PWM_init():
  global pwm_pin
  try:
    pwm_pin = m.Pwm(p)
    print("Initializing pin " + str(p) + " as PWM")
    return 0
  except ValueError:
    print(sys.exc_info()[1][0])
    return 1

class t_PWM(u.TestCase):
  def test_PWM_init(self):
    self.assertEqual(init_result,0, "PWM not successfully initialized ...")

  def test_set_PWM_period_in_useconds(self): # that is microseconds 
    self.res = pwm_pin.period_us(1000)   
    self.assertEqual(self.res, 0, "PWM us period set up failed. Please check platform.")

  def test_set_PWM_period_in_useconds_as_on_sys(self):            
    self.pwm_path = g.glob("/sys/class/pwm/pwm*/pwm" + str(p))[0]
    self.T = 1000
    pwm_pin.period_us(self.T) 
    self.T_on_system = int(c.getoutput('cat ' + self.pwm_path + '/period')) / 1000       
    self.assertEqual(self.T_on_system, self.T, "PWM us period set up failed. Please check platform")

  def test_set_PWM_period_in_miliseconds(self):           
    self.res = pwm_pin.period_ms(1)  
    self.assertEqual(self.res, 0, "PWM ms period set up failed. Please check platform")

  def test_set_PWM_period_in_miliseconds_as_on_sys(self): 
    self.pwm_path = g.glob("/sys/class/pwm/pwm*/pwm" + str(p))[0]                      
    self.T = 2                                                                 
    pwm_pin.period_ms(self.T)                                                     
    self.T_on_system = int(c.getoutput('cat ' + self.pwm_path + '/period')) / 1000000
    self.assertEqual(self.T_on_system, self.T, "PWM ms period set up failed. Please check platform")
  
  def test_PWM_enable_True(self):                                   
    self.res = pwm_pin.enable(True)  
    self.assertEqual(self.res, 0, "PWM enable(True) failed. Please check platform")

  def test_PWM_enable_True_as_on_sys(self):                                                 
    pwm_pin.enable(True) 
    self.pwm_path = g.glob("/sys/class/pwm/pwm*/pwm" + str(p))[0]
    self.en_on_sys = int(c.getoutput('cat ' + self.pwm_path + '/enable'))                                              
    self.assertEqual(self.en_on_sys, 1, "PWM enable(True) failed. Please check platform")

  def test_PWM_enable_False(self):                                                 
    self.res = pwm_pin.enable(False)                                               
    self.assertEqual(self.res, 0, "PWM enable(False) failed. Please check platform")    
                                      
  def test_PWM_enable_False_as_on_sys(self):                                       
    pwm_pin.enable(False)                                                          
    self.pwm_path = g.glob("/sys/class/pwm/pwm*/pwm" + str(p))[0]                      
    self.en_on_sys = int(c.getoutput('cat ' + self.pwm_path + '/enable'))         
    self.assertEqual(self.en_on_sys, 0, "PWM enable(False) failed. Please check platform")

  def test_PWM_write_duty_cycle(self): 
    pwm_pin.enable(True)
    self.res = pwm_pin.write(50)
    self.pwm_path = g.glob("/sys/class/pwm/pwm*/pwm" + str(p))[0] 
    self.duty_cycle_sys = c.getoutput('cat ' + self.pwm_path + '/duty_cycle')[:2]
    print str(self.duty_cycle_sys)
    self.assertEqual(int(self.duty_cycle_sys), 50, "PWM setting duty_cycle failed. Please check platform")




if __name__ == "__main__":
  p = 3
  pwm_pin = ''	
  init_result = PWM_init()
  if (init_result == 0):
    suite = u.TestLoader().loadTestsFromTestCase(t_PWM)
    u.main()
  else:
    print "Couldn't set pin " + p + "as PWM. \n"
