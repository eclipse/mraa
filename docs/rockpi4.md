Rock Pi 4 A/B Single Board Computer    {#_rockpi}
=============================

ROCK Pi 4 is a Rockchip RK3399 based SBC(Single Board Computer) by Radxa. It can run android or some Linux distributions. ROCK Pi 4 features a six core ARM processor, 64bit dual channel 3200Mb/s LPDDR4, up to 4K@60 HDMI, MIPI DSI, MIPI CSI, 3.5mm jack with mic, 802.11 ac WIFI, Bluetooth 5.0, USB Port, GbE LAN, 40-pin color expansion header, RTC. Also, ROCK Pi 4 supports USB PD and QC powering.

ROCK Pi 4 comes in two models, Model A and Model B, each model has 1GB, 2GB or 4GB ram options. for detailed difference of Model A and Model B, please check Specifications.

Board Support
-------------

- [ROCK Pi 4](https://wiki.radxa.com/Rockpi4)

Interface notes
---------------

- **UART**: ttyS0 not usable, use ttyS2 and ttyS4
-  UART2 is enabled as U-boot and Linux serial console by default. Check Rockpi4/dev/serial-console to use. Check Rockpi4/hardware/devtree_overlays to disable serial console on UART2.
-  UART2 & UART4 support a wide range of baud rate. It includes but not is not limited to the following baud rates. For instance, 115200bps. 500000bps, 1500000bps and so on.
- Function marked with color orange is the default function of this pin.
- Except Pins for power supply, all pins are laid out directly to Soc RK3399.
- For pin 3, 5, 27, 28, 29 and 31, each pin is connected to the 3.0V supply through a 4.7K pull-up resistor.
- Pin 19, 21, 23, 24 are laid out directly to the pins of SPI device on board.
- Pin 7 is laid out directly to the pin of MIPI CSI on board.
- For I2C-2 and I2C-7
- We have do the test using the i2c device e2prom. We need to open the i2c device file, and then do the read or write operation.

Pin Mapping
-----------

PROCK Pi 4 has a 40-pin expansion header. Each pin is distinguished by color.

|Additional Function |  Primary Function|  PIN  |  PIN  |  Primary Function  | Additional Function |
|--------------------|------------------|:------|------:|--------------------|---------------------|
|                    |      +3.3V       |   1   |   2   |    +5.0V           |                     |
|                    |      I2C7_SDA    |   3   |   4   |    +5.0V           |                     |
|                    |      I2C7_SCL    |   5   |   6   |    GND             |                     |
|                    |      SPI2_CLK    |   7   |   8   |    UART2_TXD	     |                     |
|                    |      GND         |   9   |   10  |    UART2_RXD       |                     |
|                    |      PWM0        |   11  |   12  |    I2S1_SCLK       |                     |
|                    |      PWM1        |   13  |   14  |    GND             |                     |
|                    |      SPDIF_TX    |   15  |   16  |    GPIO4_D2        |                     |
|                    |      +3.3V       |   17  |   18  |    GPIO4_D4        |                     |
|         UART4_TXD  |      SPI1_TXD    |   19  |   20  |    GND             |                     |
|         UART4_RXD  |      SPI1_RXD    |   21  |   22  |    GPIO4_D5        |                     |
|                    |      SPI1_CLK    |   23  |   24  |    SPI1_CSn        |                     |
|                    |      GND         |   25  |   26  |    ADC_IN0         |                     |
|                    |      I2C2_SDA    |   27  |   28  |    I2C2_CLK        |                     |
|           I2C6_SCL |      SPI2_TXD    |   29  |   30  |    GND             |                     |
|           I2C6_SDA |      SPI2_RXD    |   31  |   32  |    SPDIF_TX	     | UART3_CTSn          |
|                    |      SPI2_CSn    |   33  |   34  |    GND             |                     |
|                    |      I2S1_LRCK_TX|   35  |   36  |    I2S1_LRCK_RX    |                     |
|                    |      GPIO4_D6    |   37  |   38  |    I2S1_SDI	     |                     |
|                    |      GND         |   39  |   40  |    I2S1_SDO        |                     |


Resources
---------

The following links will take you to additional Rock Pi 4 resources

- [Armbian for Rock Pi 4](https://www.armbian.com/rock-pi-4/)
- [Forums](https://forum.radxa.com/c/rockpi4)
- [Github Repo](https://github.com/radxa)
