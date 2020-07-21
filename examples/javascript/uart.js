/*
 * Author: Eugene Bolshakov <pub@relvarsoft.com>
 * Copyright (c) 2015 Eugene Bolshakov
 *
 * SPDX-License-Identifier: MIT
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
