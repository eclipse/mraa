Galileo Rev H                           {#galileorevh}
=============

Galileo is a microcontroller board based on the Intel® Quark SoC X1000
Application Processor, a 32-bit Intel Pentium-class system on a chip.

The rev H board has the following limitations in libmraa:

- i2c is set at 400Khz speed cannot be changed without reloading kernel module
- i2c bus is shared with multiple devices, scanning it usually fails
- pwm period is set globally for all pwm channels, when changed this will halt
  all pwm channels
