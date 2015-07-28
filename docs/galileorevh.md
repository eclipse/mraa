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

Uart 1 on gen2
--------------

Uart 1 is connected to the FTDI header and the linux console. It's also
possible to use it from A2(Rx)/A3(Tx). However mraa does not support this
directly so you need to enable the muxing manually. Here is an example of how
this is done, this was tested using an FTDI 3.3V TTL cable:

$ systemctl stop serial-getty@ttyS1.service

$ python
>>> # Configure the Muxes for Uart1 on Aio2/3
>>> import mraa as m
>>> p77 = m.Gpio(77, False, True)
>>> p76 = m.Gpio(76, False, True)
>>> p16 = m.Gpio(16, False, True)
>>> p17 = m.Gpio(17, False, True)
>>> p77.write(1)
>>> p76.write(1)
>>> p16.dir(m.DIR_OUT)
>>> p16.write(0)
>>> p17.dir(m.DIR_OUT)
>>> p17.write(1)

>>> # For Rx to work correctly switch the level shifter
>>> p34 = m.Gpio(34, False, True)
>>> p34.dir(m.DIR_OUT)
>>> p34.write(1)

>>> # Use the uart
>>> x = m.Uart(1)
>>> x.setBaudRate(115200)
>>> x.writeStr('hello')
>>> x.read(5)
bytearray(b'dsds\n')

