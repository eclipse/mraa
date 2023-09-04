Radxa ROCK 3 Model C {#_Radxa}
====================

Radxa ROCK 3 Model C is a Rockchip RK3566 based SBC(Single Board Computer) by Radxa. It can run Android or Linux. Radxa ROCK 3 Model C features a four core ARM processor, 64bit dual channel 3200Mb/s LPDDR4, HDMI up to 4K60p, MIPI DSI, MIPI CSI, 3.5mm combo audio jack, Wi-Fi 6, Bluetooth 5.0, USB, GbE LAN, and 40-pin color expansion header. Radxa ROCK 3 Model C is powered by the USB Type-C port, and supports 5V input only. The recommended power adapter is 5V/3A without SSD, or 5V/5A with SSD.

Interface notes
---------------

- UART2 is enabled as the default console.
- Pin 37 is occupied by the audio I2S device, as such it is currently unavailable as GPIO.
- All UART ports support baud up to 1500000.
- Pin 3 & 5 are connected to the 3.3V supply through a 2.2K pull-up resistor to support I2C function. As such they cannot be used as GPIO.

Pin Mapping
-----------

Radxa ROCK 3 Model C has a 40-pin expansion header. Each pin is distinguished by the color.

|    Function3|  Function3|    Function2| Function1|  PIN  |  PIN  | Function1|    Function2| Function3|   Function4|
|-------------|-----------|-------------|----------|:------|------:|----------|-------------|----------|------------|
|             |           |             |       3V3|   1   |   2   |     +5.0V|             |          |            |
|             |I2C3_SDA_M0|  UART3_RX_M0|  GPIO0_D1|   3   |   4   |     +5.0V|             |          |            |
|             |I2C3_SCL_M0|  UART3_TX_M0|  GPIO1_A1|   5   |   6   |       GND|             |          |            |
|             |           |     PWM14_M0|  GPIO3_C4|   7   |   8   |  GPIO0_D1|  UART2_TX_M0|          |            |
|             |           |             |       GND|   9   |   10  |  GPIO0_D0|  UART2_RX_M0|          |            |
|             |           |             |  GPIO3_A1|   11  |   12  |  GPIO3_A3|             |          |            |
|             |           |             |  GPIO3_A2|   13  |   14  |       GND|             |          |            |
|             |           |             |  GPIO3_B0|   15  |   16  |  GPIO3_B1|  UART4_RX_M1|   PWM8_M0|            |
|             |           |             |     +3.3V|   17  |   18  |  GPIO3_B2|  UART4_TX_M1|   PWM9_M0|            |
|             |PWM15_IR_M1| SPI3_MOSI_M1|  GPIO4_C3|   19  |   20  |       GND|             |          |            |
|  UART9_TX_M1|   PWM12_M1| SPI3_MISO_M1|  GPIO4_C5|   21  |   22  |  GPIO3_C1|             |          |            |
|             |           |  SPI3_CLK_M1|  GPIO4_C2|   23  |   24  |  GPIO4_C6|  SPI3_CS0_M1|  PWM13_M1| UART9_RX_M1|
|             |           |             |       GND|   25  |   26  |        NC|             |          |            |
|             |           |  I2C4_SDA_M0|  GPIO4_B2|   27  |   28  |  GPIO4_B3|             |          |            |
|             |           |             |  GPIO3_B3|   29  |   30  |       GND|             |          |            |
|             |           |             |  GPIO3_B4|   31  |   32  |  GPIO3_C2|  UART5_TX_M1|          |            |
|             |           |  UART5_RX_M1|  GPIO3_C3|   33  |   34  |       GND|             |          |            |
|             |           |             |  GPIO3_A4|   35  |   36  |  GPIO3_A7|             |          |            |
|             |           |             |  GPIO1_A4|   37  |   38  |  GPIO3_A6|             |          |            |
|             |           |             |       GND|   39  |   40  |  GPIO3_A5|             |          |            |

Supports
--------

You can find additional product support in the following channels:

- [Product Info](https://docs.radxa.com/en/rock3/rock3c)
- [Forums](https://forum.radxa.com/c/rock3)
- [Github](https://github.com/radxa)
