Radxa ROCK 5 Model B {#_Radxa}
====================

Radxa ROCK 5 Model B is a Rockchip RK3588 based SBC(Single Board Computer) by Radxa. It can run Android or Linux. Radxa ROCK 5 Model B features a four core ARM processor, 64bit dual channel 5500Mb/s LPDDR5, HDMI up to 8K60fps, MIPI DSI, MIPI CSI, 3.5mm combo audio jack, Wi-Fi 6, Bluetooth 5.0, USB, GbE LAN, and 40-pin color expansion header. Radxa ROCK 5 Model B is powered by the USB Type-C port, and supports 5V input only. The recommended power adapter is 5V/3A without SSD, or 5V/5A with SSD.

Interface notes
---------------

- UART2 is enabled as the default console.
- All UART ports support baud up to 1500000.

Pin Mapping
-----------

Radxa ROCK 5 Model B has a 40-pin expansion header. Each pin is distinguished by the color.

|  Function5|    Function4|    Function3|    Function2|  Function1|  PIN  |  PIN  | Function1|    Function2|   Function3|   Function4|
|-----------|-------------|-------------|-------------|-----------|:------|------:|----------|-------------|------------|------------|
|           |             |             |             |        3V3|   1   |   2   |     +5.0V|             |            |            |
|           |             |  I2C7_SDA_M3|  PWM15_IR_M1|   GPIO4_B3|   3   |   4   |     +5.0V|             |            |            |
|           |             |  I2C7_SCL_M3|     PWM14_M1|   GPIO4_B2|   5   |   6   |       GND|             |            |            |
|SPI1_CS1_M1|  I2C8_SDA_M4|UART7_CTSN_M1|  PWM15_IR_M0|   GPIO3_C3|   7   |   8   |  GPIO0_B5|  UART2_TX_M0| I2C1_SCL_M0|            |
|           |             |             |             |        GND|   9   |   10  |  GPIO0_B6|  UART2_RX_M0| I2C1_SDA_M0|            |
|           |             |  SPI1_CLK_M1|  UART7_RX_M1|   GPIO3_C1|   11  |   12  |  GPIO3_B5|     PWM12_M0| UART3_TX_M1|            |
|           |             | SPI1_MOSI_M1|  I2C3_SCL_M1|   GPIO3_B7|   13  |   14  |       GND|             |            |            |
|           |  UART7_TX_M1| SPI1_MISO_M1|  I2C3_SDA_M1|   GPIO3_C0|   15  |   16  |  GPIO3_A4|  UART4_RX_M1|            |            |
|           |             |             |             |      +3.3V|   17  |   18  |  GPIO4_C4|  UART4_TX_M1|     PWM5_M2|            |
|           |             |  UART4_RX_M2| SPI0_MOSI_M2|   GPIO1_B2|   19  |   20  |       GND|             |            |            |
|           |             |     PWM12_M1| SPI0_MISO_M2|   GPIO1_B1|   21  |   22  |SARADC_IN4|             |            |            |
|           |             |  UART4_TX_M2|  SPI0_CLK_M2|   GPIO1_B3|   23  |   24  |  GPIO1_B4|  UART7_RX_M2| SPI0_CS0_M2| UART9_RX_M1|
|           |             |             |             |        GND|   25  |   26  |  GPIO1_B5|  UART7_TX_M2| SPI0_CS1_M2|            |
|           |  SPI3_CLK_M0|  I2C0_SDA_M1|   PWM7_IR_M3|   GPIO4_C6|   27  |   28  |  GPIO4_C5|             |SPI3_MOSI_M0|            |
|           |             |             |  PWM15_IR_M3|   GPIO1_D7|   29  |   30  |       GND|             |            |            |
|           |             |             |     PWM13_M2|   GPIO1_B7|   31  |   32  |  GPIO3_C2|     PWM14_M0| I2C8_SCL_M4|            |
|           |             |             |      PWM8_M0|   GPIO3_A7|   33  |   34  |       GND|             |            |            |
|           |             |  UART3_RX_M1|     PWM13_M0|   GPIO3_B6|   35  |   36  |  GPIO3_B1|      PWM2_M1| UART2_TX_M2|            |
|           |             |             |             |         NC|   37  |   38  |  GPIO3_B2|   PWM3_IR_M1| UART2_RX_M2|            |
|           |             |             |             |        GND|   39  |   40  |  GPIO3_B3|             |            |            |

Supports
--------

You can find additional product support in the following channels:

- [Product Info](https://docs.radxa.com/en/rock5/rock5b)
- [Forums](https://forum.radxa.com/c/rock5)
- [Github](https://github.com/radxa)