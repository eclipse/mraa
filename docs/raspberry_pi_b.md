Raspberry Pi Model B    {#rasppib}
====================

The Raspberry Pi is based on the Broadcom BCM2835 system on a chip,
which includes an ARM1176JZF-S 700 MHz processor, VideoCore IV GPU
and was originally shipped with 256 megabytes of RAM, later 
upgraded (Model B & Model B+) to 512 MB.

Revision Support
----------------
Currently only rev 1 is supported.

Interface notes
---------------
**GPIO** Supported on pin 6,10,11,12,14,15,17,21

**PWM** Whilst the Raspberry Pi is meant to have 1 PWM channel this is currently not supported.

**I2C** The blacklist for the kernel module will need to be removed and the `i2c-dev` needs to be loaded.

**SPI** The blacklist for the kernel module needs to be removed and `spidev` loaded

Pin Mapping
-----------

| MRAA Number | Physical Pin | Function |
|-------------|--------------|----------|
| 1           | P1-01        | 3V3 VCC  |
| 2           | P1-02        | 5V VCC   |
| 3           | P1-03        | I2C SDA  |
| 4           | P1-04        | 5V VCC   |
| 5           | P1-05        | I2C SCL  |
| 6           | P1-06        | GND      |
| 7           | P1-07        | GPIO(4)  |
| 8           | P1-08        | UART TX  |
| 9           | P1-09        | GND      |
| 10           | P1-10        | UART RX  |
| 11          | P1-11        | GPIO(17) |
| 12          | P1-12        | GPIO(18) |
| 13          | P1-13        | GPIO(21) |
| 14          | P1-14        | GND      |
| 15          | P1-15        | GPIO(22) |
| 16          | P1-16        | GPIO(23) |
| 17          | P1-17        | 3V3 VCC  |
| 18          | P1-18        | GPIO(24) |
| 19          | P1-19        | SPI MOSI |
| 20          | P1-20        | GND      |
| 21          | P1-21        | SPI MISO |
| 22          | P1-22        | GPIO(25) |
| 23          | P1-23        | SPI SCL  |
| 24          | P1-24        | SPI CS0  |
| 25          | P1-25        | GND      |
| 26          | P1-26        | SPI CS1  |
