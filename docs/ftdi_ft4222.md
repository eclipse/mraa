FTDI FT4222H                             {#ft4222}
============

The FT4222H is a High/Full Speed USB2.0-to-Quad SPI/I2C device controller. Mraa
supports it as a USB subplatform using the libft4222 library from FTDI which
can be found
[here](http://www.ftdichip.com/Support/SoftwareExamples/libft4222-1.2.1.4.tgz).
You need the latest version for the GPIO to work.

The FT4222H has 4 configuration modes selected by {DCNF1, DCNF0}.  The chip
configuration mode will determine the number of USB interfaces for data streams
and for GPIOs control. Mraa supports chip modes CNFMODE0 and CNFMODE3. In
CNFMODE0 the chip can provide either 4 GPIOs and SPI, or 2 GPIOs and I2C
since SCL/SDA are shared with GPIO0/GPIO1. It is possible to change this
selection dynamically by calling the corresponding mraa init functions.
CNFMODE3 on the other hand will only provide SPI or I2C.

By default, both modes start with I2C enabled and the driver will scan for
known GPIO expanders on the I2C bus when the FT4222H is initialized.

Supported GPIO expanders:
* PCA9672
* PCA9555
* PCF8575

Output from 'mraa-gpio list' would be as follows:
~~~~~~~~~~~~~
512 IGPIO0/SCL0: GPIO I2C
513 IGPIO1/SDA0: GPIO I2C
514   INT-GPIO2: GPIO
515   INT-GPIO3: GPIO
~~~~~~~~~~~~~

When an I2C GPIO expander is present, the pins on the expander will appear after
the 4 FT4222H GPIO pins (i.e. starting at physical pin #4, logical pin #516).
~~~~~~~~~~~~~
512 IGPIO0/SCL0: GPIO I2C
513 IGPIO1/SDA0: GPIO I2C
514   INT-GPIO2: GPIO
515   INT-GPIO3: GPIO
516   EXP-GPIO0: GPIO
517   EXP-GPIO1: GPIO
518   EXP-GPIO2: GPIO
519   EXP-GPIO3: GPIO
520   EXP-GPIO4: GPIO
521   EXP-GPIO5: GPIO
522   EXP-GPIO6: GPIO
523   EXP-GPIO7: GPIO
~~~~~~~~~~~~~

If a PCA9545 I2C switch is detected an extra four I2C busses will appear,
representing the four downstream busses. Output from 'mraa-i2c list'
would be as follows:
~~~~~~~~~~~~~
Bus 512: id=00 type=ft4222  default
Bus 513: id=01 type=ft4222
Bus 514: id=02 type=ft4222
Bus 515: id=03 type=ft4222
Bus 516: id=04 type=ft4222
~~~~~~~~~~~~~

Please note that some mraa features might not be fully implemented yet and they
are still under development (e.g. SPI replacement functions).

We tested the module using FTDI's UMFT4222EV reference board. More details on
this board can be found
[here](http://www.ftdichip.com/Support/Documents/DataSheets/Modules/DS_UMFT4222EV.pdf).

Interface notes
---------------

You will need to unload all ftdi kernel modules for libft4222 to work
correctly. You will also have to compile mraa with FT4222 support which may not
be enabled by default.

The cmake options to build this driver are:
~~~~~~~~~~~~~
FTDI4222=ON
USBPLAT=ON
~~~~~~~~~~~~~
