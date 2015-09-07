FTDI FT4222H                             {#ft4222}
============

The FT4222H is a High/Full Speed USB2.0-to-Quad SPI/I2C device controller. Mraa
supports it as a USB subplatform using the libft4222 library from FTDI which
can be found
[here](http://www.ftdichip.com/Support/SoftwareExamples/libft4222-1.2.0.240.tgz).

The FT 4222H has 4 configuration modes selected by {DCNF1, DCNF0}.  The c hip
configuration mode will determine the number of USB interface s for data stream
s and for GPIOs control. Mraa supports only chip CNFMODE0.

Whilst mraa can support custom board in CNFMODE0 (support for other mode is
welcome!) there may be some work to be done. We test using FTDI's UNFT4222EV
reference board. More detail on this board can be found
[here](http://www.ftdichip.com/Support/Documents/DataSheets/Modules/DS_UMFT4222EV.pdf).

Interface notes
---------------

You will need to unload all ftdi kernel modules for libft4222 to work
correctly. You will also have to compile mraa with FT4222 support which may not
be enabled by default.
