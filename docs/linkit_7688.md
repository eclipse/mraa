Linkit Smart 7688    {#linkit}
=================

The Linkit Smart 7688 is based on the MediaTek MT7688 system on a chip, which
includes a MIPS 24KEc 580 MHz processor and 128 megabytes of RAM.

Features:
 * 22 GPIO pins
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
| 43          | P1           | GPIO43 (Eth LED)       |
| ---         | P2           | (Eth RD+)              |
| ---         | P3           | (Eth RD-)              |
| ---         | P4           | (Eth TD+)              |
| ---         | P5           | (Eth TD-)              |
| ---         | P6           | (USB D+)               |
| ---         | P7           | (USB D-)               |
| 20          | P8           | GPIO20, UART TX2, PWM2 |
| 21          | P9           | GPIO21, UART RX2, PWM3 |
| ---         | GND          | GND                    |
| ---         | 3V3          | 3V3                    |
| ---         | VCC          | VCC                    |
| ---         | 5V           | 5V                     |
| 2           | P10          | GPIO2, (I2S WS)        |
| 3           | P11          | GPIO3, (I2S CLK)       |
| 0           | P12          | GPIO0, (I2S SDI)       |
| 1           | P13          | GPIO1, (I2S SDO)       |
| 37          | P14          | GPIO37 (REF_CLK)       |
| 44          | P15          | GPIO44 (WiFi LED)      |
| 46          | P16          | GPIO46, UART RX1       |
| 45          | P17          | GPIO45, UART TX1       |
| 13          | P18          | GPIO13, UART RX0       |
| 12          | P19          | GPIO12, UART TX0       |
| 5           | P20          | GPIO5, I2C SDA         |
| 4           | P21          | GPIO4, I2C SCL         |
| 8           | P22          | SPI MOSI               |
| 9           | P23          | SPI MISO               |
| 7           | P24          | SPI SCK                |
| 6           | P25          | GPIO6, SPI CS          |
| 18          | P26          | GPIO18, PWM0           |
| 19          | P27          | GPIO19, PWM1           |
| 17          | P28          | GPIO17, (SPIS MOSI)    |
| 16          | P29          | GPIO16, (SPIS MISO)    |
| 15          | P30          | GPIO15, (SPIS SCK)     |
| 14          | P31          | GPIO14, (SPIS CS)      |
