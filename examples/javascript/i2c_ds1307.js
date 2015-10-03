/*
 * Author: Eugene Bolshakov <pub@relvarsoft.com>
 * Copyright (c) 2015 Eugene Bolshakov
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
var sec = new Buffer(1);
var min = new Buffer(1);
var hour = new Buffer(1);
var day = new Buffer(1);
var mon = new Buffer(1);
var year = new Buffer(2);
var tbuf = new Buffer(16);


function rdtime(){
	year[0] = 0x20;
	year[1] = x.readReg(6);
	mon[0] = (x.readReg(5) & 0x1F);
	day[0] = (x.readReg(4) & 0x3F);
	hour[0] = (x.readReg(2) & 0x3F);
	min[0] = x.readReg(1);
	sec[0] = x.readReg(0);
	console.log(year.toString('hex')+'-'+mon.toString('hex')+'-'+day.toString('hex')+' '+hour.toString('hex')+':'+min.toString('hex')+':'+sec.toString('hex'));
}

function rdmem(){
	var i;
	for(i=0;i<16;i++){
		tbuf[i] = x.readReg(i+8);
	}
   console.log(tbuf.toString('hex'));
}

function wrmem(){
	var i;
	var msg = '-Data in memory-';
	for(i=0;i<16;i++){
		x.writeReg((i+8),msg.charCodeAt(i));
	}
}

console.log('MRAA Version: ' + m.getVersion()); //write the mraa version to the console

x = new m.I2c(0);
x.address(0x68);

console.log('Read current date and time: ');
rdtime();

console.log('Read first 16 bytes of memory: ');
rdmem();

console.log('Write data to memory');
wrmem();

console.log('Read first 16 bytes of memory: ');
rdmem();

