mraaStub - JavaScript simulation and stubs for mraa
====================

This project enables simulation of a device which might be accessed via mraa.
Currently this library supports I2c, SPI, and GPIO.  This project provides
several benefits:

1. Prevent crashes in nodejs applications using mraa on unsuported or
   misconfigured hardware.
2. Enable basic simulation of mraa-accessible devices for unit testing.

## Examples

The following example is based on an imaginary 'light bulb' device abstraction,
which exposes a value of brightness over a mraa-provided interface.  Please see
the `test/index.js` file for an example.

## Installation

You may install `mraaStub` from npm like this:

```
npm install mraaStub
```

Since we often switch between a mraaStub and the real mraa library, we
suggest creating an `index.js` file inside a `lib/mraaSwitcher` folder.

```js
/* index.js - file for switching between mraa and mraaStub
 */

// Define the conditions under which the mraaStub should be loaded
var platform = require('os').platform();
var m;

if (platform === 'win32') {
    m = require('mraaStub');
} else {
    m = require('mraa');
}

module.exports = m;
```

You can add this to your project in its own `lib/mraaSwitcher/index.js` file
and use `require('../mraaSwitcher')` everywhere!

## License

See [COPYING file](../COPYING) in the root of this repository.
