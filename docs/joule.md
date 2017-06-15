Intel Joule {#joule}
===========

[http://www.intel.com/joule](http://www.intel.com/joule)

The Intel Joule expansion board is supported by Mraa

Revision Support
----------------
Intel Joule expansion board

Interface notes
---------------

**SPI**

Two SPI buses are available, one with three and another one with two chipselects.
In MRAA terms an SPI "bus" (denoted by a number) is a combination
of an actual bus + specific chipselect on this bus. In view of that + taking
into account that SPI1CS1 is not available on the board, in MRAA the above
translates into five available bus numbers - from 0 to 4, covering SPI0CS0 to SPI1CS2.

Pins listed are MRAA numbered pins. You will need the `spidev` kernel module loaded
with appropriate board configuration for binding, Ref-OS-IoT does not enable this by default.

MRAA bus # | Ref-OS-IoT bus # | Joule docs bus # | MOSI | MISO | CLK | CS
-----------|------------------|------------------|------|------|-----|----
0 | 32766 | SPI0 | 67 (J13-27)| 69 (J13-29) | 65 (J13-25) | 59 (J13-19)
1 | 32766 | SPI0 | 67 (J13-27)| 69 (J13-29) | 65 (J13-25) | 61 (J13-21)
2 | 32766 | SPI0 | 67 (J13-27)| 69 (J13-29) | 65 (J13-25) | 63 (J13-23)
3 | 32765 | SPI1 | 4 (J12-4)| 2 (J12-2) | 10 (J12-10) | 6 (J12-6)
4 | 32765 | SPI1 | 4 (J12-4)| 2 (J12-2) | 10 (J12-10) | 8 (J12-8)

**UART** Some pins are labelled as UARTs but are not configured in BIOS as UART
so only available UART is on the FTDI header. Disable the getty on ttyS2 and
use mraa's uart raw mode to initialise on ttyS2. The Jumper J8 can be used to
switch between using the FTDI 6 pin header and the micro USB output.

Pin Mapping
-----------

The Intel Joule expansion board has two breakouts, breakout #1 is 1-40 whilst breakout2 is 41-80. The
LEDs are numbered from 100-103.

ISH UART are named as: IURT.
ISH I2C are named as:IIC.

| MRAA Number | Physical Pin | Function |
|-------------|--------------|----------|
| 1           | GPIO         | GPIO     |
| 2           | SPP1RX       | GPIO SPI |
| 3           | PMICRST      | NONE     |
| 4           | SPP1TX       | GPIO SPI |
| 5           | 19.2mhz      | GPIO     |
| 6           | SPP1FS0      | GPIO SPI |
| 7           | UART0TX      | GPIO     |
| 8           | SPP1FS2      | GPIO SPI |
| 9           | PWRGD        | NONE     |
| 10          | SPP1CLK      | GPIO SPI |
| 11          | I2C0SDA      | I2C      |
| 12          | I2S1SDI      | GPIO     |
| 13          | I2C0SCL      | I2C      |
| 14          | I2S1SDO      | GPIO     |
| 15          | IIC0SDA      | I2C      |
| 16          | I2S1WS       | GPIO     |
| 17          | IIC0SCL      | I2C      |
| 18          | I2S1CLK      | GPIO     |
| 19          | IIC1SDA      | I2C      |
| 20          | I2S1MCL      | GPIO     |
| 21          | IIC1SCL	     | I2C      |
| 22          | UART1TX	     | UART     |
| 23          | I2S4SDO      | NONE     |
| 24          | UART1RX      | UART     |
| 25          | I2S4SDI	     | NONE     |
| 26          | PWM0         | GPIO PWM |
| 27          | I2S4BLK      | GPIO     |
| 28          | PWM1         | GPIO PWM |
| 29          | I2S4WS       | NONE     |
| 30          | PWM2         | GPIO PWM |
| 31          | I2S3SDO      | NONE     |
| 32          | PWM3         | GPIO PWM |
| 33          | I2S3SDI      | NONE     |
| 34          | 1.8V         | NONE     |
| 35          | I2S4BLK      | GPIO     |
| 36          | GND          | NONE     |
| 37          | GND          | NONE     |
| 38          | GND          | NONE     |
| 39          | GND          | NONE     |
| 40          | 3.3V         | NONE     |
| 41          | GND          | NONE     |
| 42          | 5V           | NONE     |
| 43          | GND          | NONE     |
| 44          | 5V           | NONE     |
| 45          | GND          | NONE     |
| 46          | 3.3V         | NONE     |
| 47          | GND          | NONE     |
| 48          | 3.3V         | NONE     |
| 49          | GND          | NONE     |
| 50          | 1.8V         | NONE     |
| 51          | GPIO         | GPIO     |
| 52          | 1.8V         | NONE     |
| 53          | PANEL        | GPIO     |
| 54          | GND          | NONE     |
| 55          | PANEL        | GPIO     |
| 56          | CAMERA       | NONE     |
| 57          | PANEL        | GPIO     |
| 58          | CAMERA       | NONE     |
| 59          | SPP0FS0      | GPIO SPI |
| 60          | CAMERA       | NONE     |
| 61          | SPP0FS1      | GPIO SPI |
| 62          | SPI_DAT      | SPI      |
| 63          | SPP0FS2      | GPIO SPI |
| 64          | SPICLKB      | GPIO     |
| 65          | SPI0CLK      | GPIO SPI |
| 66          | SPICLKA      | GPIO     |
| 67          | SPP0TX       | GPIO SPI |
| 68          | UART0RX      | GPIO UART|
| 69          | SPP0RX       | GPIO SPI |
| 70          | UART0RT      | GPIO UART|
| 71          | I2C1SDA      | I2C |
| 72          | UART0CT      | GPIO UART|
| 73          | I2C1SCL      | I2C |
| 74          | IURT0TX      | GPIO UART|
| 75          | I2C2SDA      | I2C |
| 76          | IURT0RX      | GPIO UART|
| 77          | I2C2SCL      | I2C |
| 78          | IURT0RT      | GPIO UART|
| 79          | RTC_CLK      | GPIO     |
| 80          | IURT0CT      | GPIO UART|
| 100         | LED100       | GPIO     |
| 101         | LED101       | GPIO 	|
| 102         | LED102       | GPIO 	|
| 103         | LED103       | GPIO 	|
| 104         | LEDWIFI      | GPIO     |
| 105         | LEDBT        | GPIO     |

To see a live pin mapping use the command:
$ mraa-gpio list
