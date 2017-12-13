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

"use strict";

const mraa = require('mraa'); //require mraa
console.log('MRAA Version: ' + mraa.getVersion());
let uart = new mraa.Uart(0);

console.log("Note: connect Rx and Tx of UART with a wire before use");

function sleep(delay) {
    delay += new Date().getTime();
    while (new Date() < delay) {}
}

console.log("Set UART parameters");

uart.setBaudRate(115200);
uart.setMode(8, 0, 1);
uart.setFlowcontrol(false, false);
sleep(200);

console.log("First write-read circle:");

uart.writeStr("test\n");
sleep(200);
console.log(uart.readStr(6));
sleep(200);

console.log("Second write-read circle:");

uart.writeStr("2nd test\n");
sleep(200);
console.log(uart.readStr(10));
