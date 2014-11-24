Galileo Gen 2 - Rev H                           {#galileorevh}
=====================

Galileo is a microcontroller board based on the Intel(R) Quark(TM) SoC X1000
Application Processor, a 32-bit Intel Pentium-class system on a chip.

The Gen 2 board has the following limitations in libmraa:

- i2c is set at 400Khz speed cannot be changed without reloading kernel module,
  the driver is intel_qrk_gip_i2c and the parameter is i2c_std_mode which must
  be set to 1 in order to set the i2c bus speed to 100Khz
- i2c bus is shared with multiple devices in kernel space, scanning it usually
  fails
- pwm period is set globally for all pwm channels, when changed this will halt
  all pwm channels
- adc kernel module will return 12bit number but the ADC itself only has an
  accuracy of 10bits.
- AIO pins are treated as 0-5 in mraa_aio_init() but as 14-19 for everything
  else. Therefore use mraa_gpio_init(14) to use A0 as a Gpio
