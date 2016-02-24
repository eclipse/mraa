UP Board   {#up}
============

UP Board is based on the Intel(R) Atom(TM) x5-Z83xx SoC (formerly Cherry Trail).
It includes 2GB RAM, 16/32GB eMMC flash, 6 USB2.0 ports, 1 USB 3.0 OTG port,
1 Gigabit Ethernet, HDMI and DSI Graphics ports, RTC and a 40-pin I/O header.
The form-factor for the board is based on the Raspberry Pi 2, and can be used
with many of the add-on HAT boards designed for the Raspberry Pi 2.

Interface notes
---------------

**PWM** PWM frequency range is from 293 Hz to 6.4 MHz.  8-bit resolution is
supported for duty-cycle adjustments, but this reduces for frequencies > 97.6kHz

**I2C** 2 I2C channels support standard-mode (100kHz) and fast-mode (400kHz).
Bus frequency can be selected in BIOS settings.  Note that, unlike Raspberry Pi,
the I2C controller issues Repeated-START commands for combined transactions
(e.g. a write-then-read transaction) which may not be supported by some I2C
slave devices.  For such devices, it is advisable to use separate write and
read transactions to ensure that Repeated-STARTs are not issued.

**SPI** Bus frequencies up to 25MHz are supported, in steps which are less
granular at higher speeds.  E.g. Available speeds include:
 25MHz, 12.5MHz, 8.33MHz, 6.25MHz, 5MHz, 4.167MHz, 3.571MHz, 3.125MHz, etc.
Please be aware that speeds in between those steps will be rounded UP to the
next nearest available speed, and capped at 25MHz.
At the time of writing, only a single native chip-select is available (SPI CS0).

**UART** 2 high-speed UARTs are available, supporting baud rates up to
support baud rates between 300 and 3686400.  Hardware flow-control signals are
not exposed, so software flow-control may be needed for rates above 115200.
A console UART is also available but is not fully supported at this time.

Pin Mapping
-----------

The GPIO numbering in the following pin mapping is based on the Raspberry Pi
model 2 and B+ numbering scheme.

Note that there is an enum to use wiringPi style numbers.

| MRAA Number | Physical Pin | Function | Sysfs GPIO | Notes                |
|-------------|--------------|----------|------------|----------------------|
| 1           | P1-01        | 3V3 VCC  |            |                      |
| 2           | P1-02        | 5V VCC   |            |                      |
| 3           | P1-03        | I2C SDA1 |  2         | I2C1 (/dev/i2c-1)    |
| 4           | P1-04        | 5V VCC   |            |                      |
| 5           | P1-05        | I2C SCL1 |  3         | I2C1 (/dev/i2c-1)    |
| 6           | P1-06        | GND      |            |                      |
| 7           | P1-07        | GPIO(4)  |  4         |                      |
| 8           | P1-08        | UART TX1 | 14         | UART1 (/dev/ttyS1)   |
| 9           | P1-09        | GND      |            |                      |
| 10          | P1-10        | UART RX1 | 15         | UART1 (/dev/ttyS1)   |
| 11          | P1-11        | GPIO(17) | 17         |                      |
| 12          | P1-12        | I2S CLK  | 18         | I2S0 (PCM Audio)     |
| 13          | P1-13        | UART RX2 | 27         | UART2 (/dev/ttyS2)   |
| 14          | P1-14        | GND      |            |                      |
| 15          | P1-15        | GPIO(22) | 22         |                      |
| 16          | P1-16        | GPIO(23) | 23         |                      |
| 17          | P1-17        | 3V3 VCC  |            |                      |
| 18          | P1-18        | GPIO(24) | 24         |                      |
| 19          | P1-19        | SPI MOSI | 10         | SPI2 (/dev/spidev2.x)|
| 20          | P1-20        | GND      |            |                      |
| 21          | P1-21        | SPI MISO | 9          | SPI2 (/dev/spidev2.x)|
| 22          | P1-22        | UART TX2 | 25         | UART2 (/dev/ttyS2)   |
| 23          | P1-23        | SPI SCL  | 11         | SPI2 (/dev/spidev2.x)|
| 24          | P1-24        | SPI CS0  | 8          | SPI2 (/dev/spidev2.0)|
| 25          | P1-25        | GND      |            |                      |
| 26          | P1-26        | SPI CS1  | 7          | SPI2 (/dev/spidev2.1)|
| 27          | P1-27        | I2C SDA0 | 0          | I2C0 (/dev/i2c-0)    |
| 28          | P1-28        | I2C SCL0 | 1          | I2C0 (/dev/i2c-0)    |
| 29          | P1-29        | GPIO(5)  | 5          |                      |
| 30          | P1-30        | GND      |            |                      |
| 31          | P1-31        | GPIO(6)  | 6          |                      |
| 32          | P1-32        | PWM0     | 12         | PWM Chip 0 Channel 0 |
| 33          | P1-33        | PWM1     | 13         | PWM Chip 1 Channel 0 |
| 34          | P1-34        | GND      |            |                      |
| 35          | P1-35        | I2S FRM  | 19         | I2S0 (PCM Audio)     |
| 36          | P1-36        | GPIO(16) | 16         |                      |
| 37          | P1-37        | GPIO(26) | 26         |                      |
| 38          | P1-38        | I2S DIN  | 20         | I2S0 (PCM Audio)     |
| 39          | P1-39        | GND      |            |                      |
| 40          | P1-40        | I2S DOUT | 21         | I2S0 (PCM Audio)     |
