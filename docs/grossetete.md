Grosse Tete {#grossetete}
===========

The Grosse Tete with the Tuchuck board is supported by Mraa

Revision Support
----------------
Tuchuck

Interface notes
---------------

**SPI** Currently not working

**UART** Some pins are labelled as UARTs but are not configured in BIOS as UART
so only available UART is on the FTDI header

Pin Mapping
-----------

Tuchuck has two breakouts, breakout #1 is 1-40 whilst breakout2 is 41-80. The
LEDs are numbered from 100-103.

| MRAA Number | Physical Pin | Function |
|-------------|--------------|----------|
| 1           | GPIO         | GPIO     |
| 2           | SPP1RX       | GPIO     |
| 3           | PMICRST      | NONE     |
| 4           | SPP1TX       | GPIO     |
| 5           | 19.2mhz      | GPIO     |
| 6           | SPP1FS0      | GPIO     |
| 7           | UART0TX      | GPIO     |
| 8           | SPP1FS2      | GPIO     |
| 9           | PWRGD        | NONE     |
| 10          | SPP1CLK      | GPIO     |
| 11          | I2C0SDA      | I2C      |
| 12          | I2S1SDI      | GPIO     |
| 13          | I2C0SCL      | I2C      |
| 14          | I2S1SDO      | GPIO     |
| 15          | I2C1SDA      | I2C      |
| 16          | I2S1WS       | GPIO     |
| 17          | I2C1SCL      | I2C      |
| 18          | I2S1CLK      | GPIO     |
| 19          | I2C2SDA      | I2C      |
| 20          | I2S1MCL      | GPIO     |
| 21          | I2C2SCL	     | I2CO     |
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
| 59          | SPP0FS0      | GPIO     |
| 60          | CAMERA       | NONE     |
| 61          | SPP0FS1      | GPIO     |
| 62          | SPI_DAT      | SPI      |
| 63          | SPP0FS2      | GPIO     |
| 64          | SPICLKB      | GPIO SPI |
| 65          | SPP0FS3      | GPIO     |
| 66          | SPICLKA      | GPIO SPI |
| 67          | SPP0TX       | GPIO     |
| 68          | UART0RX      | GPIO UART|
| 69          | SPP0RX       | GPIO     |
| 70          | UART0RT      | GPIO UART|
| 71          | I2C1SDA      | GPIO I2C |
| 72          | UART0CT      | GPIO UART|
| 73          | I2C1SCL      | GPIO I2C |
| 74          | UART1TX      | GPIO UART|
| 75          | I2C2SDA      | GPIO I2C |
| 76          | UART1RX      | GPIO UART|
| 77          | I2C1SCL      | GPIO I2C |
| 78          | UART1RT      | GPIO UART|
| 79          | RTC_CLK      | GPIO     |
| 80          | UART1CT      | GPIO UART|
| 100         | LED100       | GPIO     |
| 101         | LED101       | GPIO 	|
| 102         | LED102       | GPIO 	|
| 103         | LED103       | GPIO 	|

To see a live pin mapping use the command:
$ mraa-gpio list
