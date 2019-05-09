/*
 * Author: Henry Bruce <henry.bruce@intel.com>
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

"use strict";

const mraa = require('mraa');
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
    let pinNumber = arguments[0];
    let pinValue = arguments[1];
    let pin = new mraa.Gpio(pinNumber);
    pin.dir(mraa.DIR_OUT);
    pin.write(pinNumber, pinValue);
}

function getPin() {
    let pinNumber = arguments[0];
    let pin = new mraa.Gpio(pinNumber);
    pin.dir(mraa.DIR_IN);
    console.log('Gpio ' + pinNumber + ' = ' + pin.read());
}

function onPinLevelChange() {
    console.log('gpio level change');
}

function monitorPin() {
    let pinNumber = arguments[0];
    try {
        let pin = new mraa.Gpio(pinNumber);
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
    if (argc > 3) {
        const pinNumber = parseInt(args[3]);
    }

    switch (args[2]) {
        case "version":
            getVersion();
            break;
        case "get":
            getPin(pinNumber);
            break;
        case "set":
            let pinValue = parseInt(args[4]);
            getPin(pinNumber, pinValue);
            break;
        case "monitor":
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
