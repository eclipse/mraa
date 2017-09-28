Onion Omega2    {#omega2}
============

The Omega2 board is based on the MediaTek MT7688 system on a chip, which
includes a MIPS 24KEc 580 MHz processor and 128 megabytes of RAM.

Features:
 * 15 GPIO pins
 * 2 PWM pins
 * 2 UART ports
 * 1 SPI master port
 * 1 I2C port
 * 1 I2S port (not supported by libmraa)

![Pinout](https://raw.githubusercontent.com/OnionIoT/Onion-Docs/master/Omega2/Documentation/Hardware-Overview/img/Omega-2-Pinout-Diagram.png)


Revision Support
----------------
 * Omega2
 * Omega2+


Pin Mapping
-----------

| MRAA Number | Function               | Function               | MRAA Number |
|-------------|------------------------|------------------------|-------------|
| ---         | GND                    | GND                    | ---         |
| 11          | GPIO11                 | VIN 3V3                | ---         |
| 3           | GPIO3, (I2S CLK)       | (USB D+)               | ---         |
| 2           | GPIO2, (I2S WS)        | (USB D-)               | ---         |
| 17          | GPIO17                 | GPIO13, UART RX0       | 13          |
| 16          | GPIO16                 | GPIO12, UART TX0       | 12          |
| 15          | GPIO15                 | FW_RST                 | 38          |
| 46          | GPIO46, UART RX1       | VOUT 3V3               | ---         |
| 45          | GPIO45, UART TX1       | (Eth TX-)              | ---         |
| 9           | SPI MISO               | (Eth TX+)              | ---         |
| 8           | SPI MOSI               | (Eth RX-)              | ---         |
| 7           | SPI SCK                | (Eth RX+)              | ---         |
| 6           | GPIO6, SPI CS          | GPIO18, PWM0           | 18          |
| 1           | GPIO1, (I2S SDO)       | GPIO19, PWM1           | 19          |
| 0           | GPIO0, (I2S SDI)       | GPIO4, I2C SCL         | 4           |
| ---         | (Reset)                | GPIO5, I2C SDA         | 5           |
