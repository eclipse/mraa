/*
 * Author: David Antler <david.a.antler@intel.com>
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

