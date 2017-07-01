#!/usr/bin/env lua

-- Author: Vasiliy Soshnikov <v.soshnikov@corp.mail.com, dedok.mad@gmail.com>
-- Copyright (c) 2016 Intel Corporation.
--
-- Permission is hereby granted, free of charge, to any person obtaining
-- a copy of this software and associated documentation files (the
-- "Software"), to deal in the Software without restriction, including
-- without limitation the rights to use, copy, modify, merge, publish,
-- distribute, sublicense, and/or sell copies of the Software, and to
-- permit persons to whom the Software is furnished to do so, subject to
-- the following conditions:
--
-- The above copyright notice and this permission notice shall be
-- included in all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
-- EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
-- MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
-- NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
-- LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
-- OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
-- WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

mraa = require('mraa')
os = require('os')

local function my_sleep(sec)
  os.execute('sleep ' .. tonumber(sec))
end

x = mraa.Pwm(3)
x:period_us(700)
x:enable(true)
value = 0.0

while true do
  x:write(value)
  my_sleep(0.05)
  value = value + 0.01
  if value >= 1 then
    value = 0.0
  end
end
