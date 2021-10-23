UP Xtreme  Board   {#up_xtreme}
================================
UP Xtreme is based on the Intel&reg; Core&trade; i3/i5/i7 Whiskey Lake SoCs.

For the full specification please refer to the main specification page here:

https://up-board.org/wp-content/uploads/up-xtreme/Datasheet-UP-xtreme.pdf

Interface notes
-----------------------
The UP Xtreme present one Raspberry Pi compatible HAT connector and a  100 pin exHAT connector. Currently this implementation only support the interfaces through the HAT connector.

**I2C**
 - 2 channels
 - Support: standard-mode (100kHz), fast-mode (400kHz), Fast-mode plus (1MHz), High-speed mode (3.4MHz)
 - Bus frequency can be selected in BIOS settings
 - The default i2c channel is the one connected to the pin 3,5 of the hat
 - On some OSes the i2c-dev kernel module may need to be loaded manually to export the i2c device nodes

**SPI**
 - Bus frequencies up to 10MHz are supported
 - 2 chip-selects
 - To enable SPI device nodes the ACPI tables need to be overwritten as explained [here](https://wiki.up-community.org/Pinout_UP2#SPI_Ports)

**UART**
 - 1 high-speed UART is available
 - Supporting baud rates up to 3686400 baud
 - Hardware flow-control signals are available on pins 11/36 (RTS/CTS)

Please note that a kernel with UP board support is required to enable the I/O
interfaces above.

Refer to http://www.up-community.org for more information.

Pin Mapping
--------------------
The GPIO numbering in the following pin mapping is based on the Raspberry Pi
model 2 and B+ numbering scheme.

NOTE: the i2c device numbering depend on various factor and cannot be trusted:
the right way of determining i2c (and other devices) numbering is through PCI
physical device names. See the source code in src/x86/up_xtreme.c for details.

| MRAA no. | Function     | Rpi GPIO   | Sysfs GPIO | mraa device     |
|----------|--------------|------------|------------|-----------------|
| 1        | 3V3 VCC      |            |            |                 |
| 2        | 5V VCC       |            |            |                 |
| 3        | I2C_SDA      | 2          | 368        | I2C0            |
| 4        | 5V VCC       |            |            |                 |
| 5        | I2C_SCL      | 3          | 369        | I2C0            |
| 6        | GND          |            |            |                 |
| 7        | GPIO(4)      | 4          | 296        |                 |
| 8        | UART_TX      | 14         | 469        | UART0           |
| 9        | GND          |            |            |                 |
| 10       | UART_RX      | 15         | 468        | UART0           |
| 11       | UART_RTS     | 17         | 470        | UART0           |
| 12       | I2S_CLK      | 18         | 360        |                 |
| 13       | GPIO(27)     | 27         | 268        |                 |
| 14       | GND          |            |            |                 |
| 15       | GPIO(22)     | 22         | 270        |                 |
| 16       | GPIO(23)     | 23         | 264        |                 |
| 17       | 3V3 VCC      |            |            |                 |
| 18       | GPIO(24)     | 24         | 265        |                 |
| 19       | SPI0_MOSI    | 10         | 308        | SPI0            |
| 20       | GND          |            |            |                 |
| 21       | SPI0_MISO    | 9          | 307        | SPI0            |
| 22       | GPIO(25)     | 25         | 266        |                 |
| 23       | SPI0_SCL     | 11         | 306        | SPI0            |
| 24       | SPI0_CS0     | 8          | 305        | SPI0            |
| 25       | GND          |            |            |                 |
| 26       | SPI0_CS1     | 7          | 311        | SPI0            |
| 27       | ID_SD        | 0          | 366        | I2C1            |
| 28       | ID_SC        | 1          | 367        | I2C1            |
| 29       | GPIO(5)      | 5          | 317        |                 |
| 30       | GND          |            |            |                 |
| 31       | GPIO(6)      | 6          | 318        |                 |
| 32       | GPIO(12)     | 12         | 298        |                 |
| 33       | GPIO(13)     | 13         | 299        |                 |
| 34       | GND          |            |            |                 |
| 35       | I2S_FRM      | 19         | 261        |                 |
| 36       | UART_CTS     | 16         | 471        | UART0           |
| 37       | GPIO(26)     | 26         | 267        |                 |
| 38       | I2S_DIN      | 20         | 363        |                 |
| 39       | GND          |            |            |                 |
| 40       | I2S_DOUT     | 21         | 362        |                 |
