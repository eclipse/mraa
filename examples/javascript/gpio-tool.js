/*
 * Author: Henry Bruce <henry.bruce@intel.com>
 * Copyright (c) 2016 Intel Corporation.
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

var mraa = require('mraa');
const readline = require('readline');

const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout
});

function printUsage() {
    console.log("version         Print version");
    console.log("get pin         Get pin level");
    console.log("set pin level   Set pin level");
    console.log("monitor pin     Monitor pin level changes");
}

function getVersion() {
    console.log('MRAA Version: ' + mraa.getVersion());
}

function setPin() {
    var pinNumber = arguments[0];
    var pinValue = arguments[1];
    var pin = new mraa.Gpio(pinNumber);
    pin.dir(mraa.DIR_OUT);
    pin.write(pinNumber, pinValue);
}

function getPin() {
    var pinNumber = arguments[0];
    var pin = new mraa.Gpio(pinNumber);
    pin.dir(mraa.DIR_IN);
    console.log('Gpio ' + pinNumber + ' = ' + pin.read());
}

function onPinLevelChange() {
    console.log('gpio level change');
}

function monitorPin() {
    var pinNumber = arguments[0];
    try {
        var pin = new mraa.Gpio(pinNumber);
        pin.dir(mraa.DIR_IN);
        pin.isr(mraa.EDGE_BOTH, onPinLevelChange);
        rl.question('Press ENTER to stop', function(answer) {
            rl.close();
            pin.isrExit();
        });
    } catch (err) {
     console.log(err.message);
    }
}

const args = process.argv;
const argc = args.length;
if (argc >= 3) {
    const cmd = args[2];
    switch (args[2]) {
    case "version":
        getVersion();
        break;
    case "get":
        var pinNumber = parseInt(args[3]);
        getPin(pinNumber);
        break;
    case "set":
        var pinNumber = parseInt(args[3]);
        var pinValue = parseInt(args[4]);
        getPin(pinNumber, pinValue);
        break;
    case "monitor":
        var pinNumber = parseInt(args[3]);
        monitorPin(pinNumber);
        break;
    default:
        console.log("Invalid command " + args[2]);
        break;
    }
} else {
    console.log("Command not specified");
    printUsage();
}

