#!/usr/bin/env node

/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

var m = require('mraa'); //require mraa

// helper function to go from hex val to dec
function char(x) { return parseInt(x, 16); }

x = new m.I2c(0)
x.address(0x77)

// initialise device
if (x.readReg(char('0xd0')) != char('0x55')) {
  console.log("error");
}

// we want to read temperature so write 0x2e into control reg
x.writeReg(char('0xf4'), char('0x2e'))

// read a 16bit reg, obviously it's uncalibrated so mostly a useless value :)
console.log(x.readWordReg(char('0xf6')))

// and we can do the same thing with the read()/write() calls if we wished
// thought I'd really not recommend it!
buf = new Buffer(2)
buf[0] = char('0xf4')
buf[1] = char('0x2e')
console.log(buf.toString('hex'))
x.write(buf)

x.writeByte(char('0xf6'))
d = x.read(2)
