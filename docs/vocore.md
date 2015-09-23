VoCore              {#vocore}
======

VoCore is a coin-sized Linux computer with wifi. It is also able to
work as a full functional router. It runs OpenWrt on top of Linux. 
It contains 32MB SDRAM, 8MB SPI Flash and using RT5350(360MHz MIPS)
as its heart. It provides many interfaces such as 10/100M Ethernet,
USB, UART, I2C, I2S, PCM, JTAG and over 20 GPIOs but its size is 
less than one square inch(25mm x 25mm).

Revision Support
----------------
Currently only rev 1 is supported.

Interface notes
---------------
**GPIO** Where a pin is not used for UART/I2C/SPI gpio can be used 
on it

**I2C** The `i2c-dev` module needs to be loaded. Currently there are
issues with I2C so dont expect it to work.

**SPI** The VoCore has two chip selects. CS0 is used for the SPI 
storage onboard. CS1 when used with the dock is used with the 
micro-SD card slot.

Pin Mapping
-----------

| MRAA Pin | VoCore Description | Used Function |
|----------|--------------------|---------------|
| 1        | GND                |               |
| 2        | RESET              |               |
| 3        | G#02               | I2C_SD        |
| 4        | G#01               | I2C_SCLK      |
| 5        | G#18               | JTAG_TDI      |
| 6        | G#00               |               |
| 7        | G#19               | JTAG_TMS      |
| 8        | G#20               | JTAG_TCLK     |
| 9        | G#21               | JTAG_TRST     |
| 10       | G#17               | JTAG_TDO      |
| 11       | GND                |               |
| 12       | VIN                | 3.5V TO 6.0V  |
| 13       | VIN                | 3.5V TO 6.0V  |
| 14       | GND                |               |
| 15       | G#12               | PCM_CLK       |
| 16       | G#09               | I2S_SDO       |
| 17       | G#22               | LED0          |
| 18       | G#08               | I2S_WS/TXD    |
| 19       | 1V8                |               |
| 20       | G#04               | SPI_CLK       |
| 21       | G#05               | SPI_MOSI      |
| 22       | G#27               | SPI_CSI       |
| 23       | G#23               | LED1          |
| 24       | G#13               | PCM_DRX       |
| 25       | G#24               | LED2          |
| 26       | G#25               | LED3          |
| 27       | G#26               | LED4          |
| 28       | G#14               | PCM_DTX       |
| 29       | GND                |               |
| 30       | G#15               | TXD2          |
| 31       | G#07               | I2S_CLK       |
| 32       | G#16               | RXD2          |
| 33       | G#10               | I2S_SDI/RXD   |
| 34       | G#06               | SPI_MISO      |
| 35       | G#03               | SPI_CS0       |
| 36       | G#11               | PCM_FS        |
