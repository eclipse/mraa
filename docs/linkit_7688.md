Linkit Smart 7688    {#linkit_7688}
=================

The Linkit Smart 7688 is based on the MediaTek MT7688 system on a chip, which
includes a MIPS 24KEc 580 MHz processor and 128 megabytes of RAM.

Features:
 * 20 GPIO pins
 * 4 PWM pins
 * 3 UART ports
 * 1 SPI master port
 * 1 SPI slave port (not supported by libmraa)
 * 1 I2C port
 * 1 I2S port (not supported by libmraa)

![Pinout](http://www.cnx-software.com/wp-content/uploads/2015/12/Link_Smart_7688_Pinout.png)


Revision Support
----------------
 * Linkit Smart 7688
 * Linkit Smart 7688 Duo


Pin Mapping
-----------

| MRAA Number | Physical Pin | Function               |
|-------------|--------------|------------------------|
| ---         | P0           | (Reset)                |
| 1           | P1           | GPIO43 (Eth LED)       |
| ---         | P2           | (Eth RD+)              |
| ---         | P3           | (Eth RD-)              |
| ---         | P4           | (Eth TD+)              |
| ---         | P5           | (Eth TD-)              |
| ---         | P6           | (USB D+)               |
| ---         | P7           | (USB D-)               |
| 8           | P8           | GPIO20, UART TX2, PWM2 |
| 9           | P9           | GPIO21, UART RX2, PWM3 |
| ---         | GND          | GND                    |
| ---         | 3V3          | 3V3                    |
| ---         | VCC          | VCC                    |
| ---         | 5V           | 5V                     |
| 10          | P10          | GPIO2, (I2S WS)        |
| 11          | P11          | GPIO3, (I2S CLK)       |
| 12          | P12          | GPIO0, (I2S SDI)       |
| 13          | P13          | GPIO1, (I2S SDO)       |
| 14          | P14          | GPIO37 (REF_CLK)       |
| 15          | P15          | GPIO44 (WiFi LED)      |
| 16          | P16          | GPIO46, UART RX1       |
| 17          | P17          | GPIO45, UART TX1       |
| 18          | P18          | GPIO13, UART RX0       |
| 19          | P19          | GPIO12, UART TX0       |
| 20          | P20          | GPIO5, I2C SDA         |
| 21          | P21          | GPIO4, I2C SCL         |
| 22          | P22          | SPI MOSI               |
| 23          | P23          | SPI MISO               |
| 24          | P24          | SPI SCK                |
| 25          | P25          | GPIO6, SPI CS          |
| 26          | P26          | GPIO18, PWM0           |
| 27          | P27          | GPIO19, PWM1           |
| 28          | P28          | GPIO17, (SPIS MOSI)    |
| 29          | P29          | GPIO16, (SPIS MISO)    |
| 30          | P30          | GPIO15, (SPIS SCK)     |
| 31          | P31          | GPIO14, (SPIS CS)      |
