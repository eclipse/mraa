Banana Pi/Pro    {#bananapi}
============

The Banana Pi/Pro is a clone of the well known Raspberry Pi. It has advantages
compared to the 'old' Raspberry Pi A/B/A+/B+ devices as it is based on the
Allwinner A20 Dual Core Cortex-A7. It also has 1G Ethernet compared to 100M
Ethernet on the Raspberry.

There is a lot of effort on Mainline Kernel to fully support the Allwinner
Chips, for example Fedora 22 Arm will come with support for BananaPi/Pro which
means that you will be able to have HDMI Output and a number of periperal
modules work out of the box.

Pin-wise the Banana Pi matches the Raspberry Pi, the Banana Pro has a connector
similar to the Raspberry Pi A+/B+

Revision Support
----------------
Banana Pi
Banana Pro

Interface notes
---------------

**PWM** Whilst the Banana Pi is meant to have 1 PWM channel this is currently
not supported.

**SPI** works fine when used with old 3.4 Kernels provided by Lemaker, on
Mainline Kernel SPI does currently not work

**COM** I have created devicetree patches so that Mainline Kernel supports all
COM-Interfaces, this is not yet visible in Kernel

Mainline Kernel requires the use of Device-Trees, mraa tries it's best to guess
which gpio/serial/i2c/spi is connected where but there is currently no support
to manipulate the Device-Tree settings from within mraa. If a device does not
work as expected then please check syslog, mraa usually complains with a
meaningful message when it is unable to initialize the device.

Pin Mapping
-----------

This pin mapping refers to the Banana Pi but gives an idea
as to what the values are from mraa. Note that there is an emum to use wiringPi
style numbers.

| MRAA Number | Physical Pin | Function  |
|-------------|--------------|-----------|
| 1           | P1-01        | 3V3 VCC   |
| 2           | P1-02        | 5V VCC    |
| 3           | P1-03        | I2C SDA   |
| 4           | P1-04        | 5V VCC    |
| 5           | P1-05        | I2C SCL   |
| 6           | P1-06        | GND       |
| 7           | P1-07        | GPIO(PI03)|
| 8           | P1-08        | UART4 TX  |
| 9           | P1-09        | GND       |
| 10          | P1-10        | UART4 RX  |
| 11          | P1-11        | GPIO(PI19)|
| 12          | P1-12        | GPIO(PH02)|
| 13          | P1-13        | GPIO(PI18)|
| 14          | P1-14        | GND       |
| 15          | P1-15        | GPIO(PI17)|
| 16          | P1-16        | GPIO(PH20)|
| 17          | P1-17        | 3V3 VCC   |
| 18          | P1-18        | GPIO(PH21)|
| 19          | P1-19        | SPI MOSI  |
| 20          | P1-20        | GND       |
| 21          | P1-21        | SPI MISO  |
| 22          | P1-22        | GPIO(PI16)|
| 23          | P1-23        | SPI SCL   |
| 24          | P1-24        | SPI CS0   |
| 25          | P1-25        | GND       |
| 26          | P1-26        | SPI CS1   |

There is also a second 8-pin connector on the Banana Pi, the pins are as follows:

| 27          | P1-19        | 5V VCC    |
| 28          | P1-20        | 3V3 VCC   |
| 29          | P1-21        | GPIO(PH5) |
| 30          | P1-22        | GPIO(PI22)|
| 31          | P1-23        | GPIO(PH03)|
| 32          | P1-24        | GPIO(PI20)|
| 33          | P1-25        | GND       |
| 34          | P1-26        | GND       |
