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
 *
 *
 * @fileoverview Implementation of a LightBulb class abstraction
 *
 */

module.exports = (function() {
    "use strict";
    var m = require('../index');

    /**
     * Constructor for a new LightBulb
     * @class LightBulb
     *
     * @classdesc This class abstracts access to the control and data registers
     * on an imaginary lightbulb.
     * @param {Object} A libmraa I2c object, initialized
     */
    function LightBulb (i2cInterface) {
        var self = this;
        self._i2cInterface = i2cInterface;

        self.getBrightness = function() {
            // Presume our brightness data is one byte at offset 4
            return self._i2cInterface.readReg(4);
        }

        return self;
    }

    return LightBulb;
})();

