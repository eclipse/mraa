/*
 * Author: David Antler <david.a.antler@intel.com>
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 *
 * @fileoverview This file implements a fake mraa stub which enables testing
 * as well as the ability to run on Windows.
 */

var m;
var winston = require('winston');
var logger = new winston.Logger({
        transports: [
            new winston.transports.Console({
                level: 'error',
                handleExceptions: false,
                json: false,
                colorize: true})
            ],
        exitOnError: false
    });

/**
 * @class mraaStub
 * @classdesc This class is designed to stub out libmraa so we can run
 * test code on unsupported platforms (specifically Windows).
 */
var mraaStub = function() {
    var verison = '0.0.1';
    var self = this;
    self.EDGE_BOTH = 1;
    self.EDGE_NONE = 2;
    self.EDGE_RISING = 3;
    self.EDGE_FALLING = 4;
    self.DIR_IN = 1;
    self.DIR_OUT = 2;

    self.getVersion = function() {
        return "mraaStub " + version;
    };

    // Stub out GPIO
    function Gpio(num) {
        this.num = num;
        this._callback = null;
        this._dir = null;
        this._isr_mode = self.EDGE_NONE;
    }

    var logGpio = false;
    Gpio.prototype._callIsr = function() {
        if(this.isr_mode === self.EDGE_NONE) {
            logger.log('info',"Could not call ISR.  Not set up for triggering");
        }
        this._callback();
    };

    Gpio.prototype.isr = function(mode, handler){
        if(logGpio) {
            logger.log('info',"GPIO " + this.num + " isr stub invoked.");
        }
        this._isr_mode = self.EDGE_NONE;
        this._callback = handler;
    };

    Gpio.prototype.dir = function(d) {
        if(logGpio) {
            logger.log('info',"GPIO " + this.num + " dir stub invoked.");
        }
        this._dir = d;
    };
    Gpio.prototype.write = function(z) {
        if(logGpio) {
            logger.log('logger',"GPIO " + this.num + " write stub invoked.");
        }
        if(this._dir !== self.DIR_OUT) {
            logger.log('info',"GPIO " + this.num + " write called without DIR_OUT set.");
        }
    };
    Gpio.prototype.read = function() {
        if(logGpio) {
            logger.log('info',"GPIO " + this.num + " read stub invoked.");
        }
        return 0;
    };

    // Stub out SPI
    function Spi(num) {
        var self = this;
        this.num = num;
        this._buffer = new Buffer(29);
        this._buffer.fill(0);
        this._loopback = false;
    }

    Spi.prototype._setOutput = function(buf) {
        this._buffer = buf;
    };

    Spi.prototype._enableLoopback = function(x) {
        if(x === true) {
            this._loopback = true;
        } else {
            this._loopback = false;
        }
    };

    Spi.prototype.write = function(b) {
        logger.log('info',"SPI write stub invoked.");
        if(this._loopback === true) {
            return b;
        }
        return new Buffer(this._buffer);
    };

    Spi.prototype.frequency = function(f) {
        logger.log('info',"SPI frequency stub invoked.");
        return f;
    };

    Spi.prototype.lsbmode = function(t) {
        logger.log('info',"SPI lsbmode stub invoked.");
    };

    Spi.prototype.mode = function(x) {
        logger.log('info',"SPI mode stub invoked.");
    };

    function I2c(num) {
        this._num = num;
        this._regMapInitialized = false;
    }

    /* This function sets an internal register map for the I2c device.
     */
    I2c.prototype._setRegisterMapInternal = function(buffer) {
        this._regMapInitialized = true;
        this._buffer = buffer;
    };

    I2c.prototype.frequency = function(freq) {
        // Do nothing.  We don't care.
    };

    I2c.prototype.address = function(address) {
        var self = this;
        self.address = address;
    };

    I2c.prototype.readReg = function(regAddr) {
        if(!this._regMapInitialized) {
            logger.log('error', "Need to set reg map");
        }
        if(!this.address) {
            logger.log('error', "Need to set address");
        }

        return this._buffer.readUInt8(regAddr);
    };

    I2c.prototype.readWordReg = function(regAddr) {
        if(!this._regMapInitialized) {
            logger.log('error', "Need to set reg map");
        }
        if(!this.address) {
            logger.log('error', "Need to set address");
        }

        return this._buffer.readUInt16LE(regAddr);
    };

    I2c.prototype.readBytesReg = function(regAddr, len) {
        if(!this._regMapInitialized) {
            logger.log('error', "Need to set reg map");
        }
        if(!this.address) {
            logger.log('error', "Need to set address");
        }

        return this._buffer.slice(regAddr,regAddr+len);
    };

    I2c.prototype.write = function(buf) {
        if(!this._regMapInitialized) {
            logger.log('error', "Need to set reg map");
        }
        if(!this.address) {
            logger.log('error', "Need to set address");
        }

        var regAddr = buf[0];
        var newBuf = buf.slice(1);
        newBuf.copy(this._buffer, regAddr);
    };

    I2c.prototype.writeReg = function(regAddr, data) {
        if(!this._regMapInitialized) {
            logger.log('error', "Need to set reg map");
        }
        if(!this.address) {
            logger.log('error', "Need to set address");
        }

        this._buffer.writeUInt8(regAddr,data);
    };

    I2c.prototype.writeWordReg = function(regAddr, dataWord) {
        if(!this._regMapInitialized) {
            logger.log('error', "Need to set reg map");
        }
        if(!this.address) {
            logger.log('error', "Need to set address");
        }

        this._buffer.writeUInt16LE(regAddr,data);
    };

    // Export our stubs
    self.Gpio = Gpio;
    self.Spi = Spi;
    self.I2c = I2c;

};

m = new mraaStub();

module.exports = m;
