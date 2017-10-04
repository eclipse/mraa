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

Left side:

| MRAA Number | Function               |
|-------------|------------------------|
| ---         | GND                    |
| 1           | GPIO11                 |
| 2           | GPIO3, (I2S CLK)       |
| 3           | GPIO2, (I2S WS)        |
| 4           | GPIO17                 |
| 5           | GPIO16                 |
| 6           | GPIO15                 |
| 7           | GPIO46, UART RX1       |
| 8           | GPIO45, UART TX1       |
| 9           | SPI MISO               |
| 10          | SPI MOSI               |
| 11          | SPI SCK                |
| 12          | GPIO6, SPI CS          |
| 13          | GPIO1, (I2S SDO)       |
| 14          | GPIO0, (I2S SDI)       |
| ---         | (Reset)                |

Right side:

| MRAA Number | Function               |
|-------------|------------------------|
| ---         | GND                    |
| ---         | VIN 3V3                |
| ---         | (USB D+)               |
| ---         | (USB D-)               |
| 20          | GPIO13, UART RX0       |
| 21          | GPIO12, UART TX0       |
| 22          | FW RST                 |
| ---         | VOUT 3V3               |
| ---         | (Eth TX-)              |
| ---         | (Eth TX+)              |
| ---         | (Eth RX-)              |
| ---         | (Eth RX+)              |
| 28          | GPIO18, PWM0           |
| 29          | GPIO19, PWM1           |
| 30          | GPIO4, I2C SCL         |
| 31          | GPIO5, I2C SDA         |
