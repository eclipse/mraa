/*
 * Author: David Antler <david.a.antler@intel.com>
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

var expect = require('expect.js');
var m = require('../index');
var LightBulb = require('./lightbulb');

describe('LightBulb', function() {

    /** Model the internal data of LightBulb as a buffer */
    var bufferFullBrightness = new Buffer(
       [ 'N',   // Four bytes allocated for name
         'a',
         'm',
         'e',
          95    // One byte allocated for brightness. Stuff in '95' value!
       ]);

    it('getBrightness() function should return 95', function() {

        // Create a fake I2c bus based on the 'full brightness' data model
        var testI2cBus = new m.I2c(0);
        testI2cBus._setRegisterMapInternal(bufferFullBrightness);

        // Create a new LightBulb that opens the testI2cBus, instead of a real
        // mraa I2c bus.
        var lightBulbI2c = new LightBulb(testI2cBus);

        // presumably getBrightness will gather data from I2C and get '95'
        var brightness = lightBulbI2c.getBrightness();

        expect(brightness).to.be(95);
    })
});

