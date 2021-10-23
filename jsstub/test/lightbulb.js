/*
 * Author: David Antler <david.a.antler@intel.com>
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
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

