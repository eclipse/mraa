Radxa ROCK 3 Model A {#_Radxa}
====================

Radxa ROCK 3 Model A is a Rockchip RK3568 based SBC(Single Board Computer) by Radxa. It can run Android or Linux. Radxa ROCK 3 Model A features a four core ARM processor, 64bit dual channel 3200Mb/s LPDDR4, HDMI up to 4K60p, MIPI DSI, MIPI CSI, 3.5mm combo audio jack, Wi-Fi 6, Bluetooth 5.0, USB, GbE LAN, and 40-pin color expansion header. Radxa ROCK 3 Model A is powered by the USB Type-C port, and supports 5V input only. The recommended power adapter is 5V/3A without SSD, or 5V/5A with SSD.

Interface notes
---------------

- UART2 is enabled as the default console.
- All UART ports support baud up to 1500000.

Pin Mapping
-----------

Radxa ROCK 3 Model A has a 40-pin expansion header. Each pin is distinguished by the color.

|    Function5|    Function4|  Function3|    Function2|  Function1|  PIN  |  PIN  | Function1|    Function2|  Function3|   Function4|   Function5|
|-------------|-------------|-----------|-------------|-----------|:------|------:|----------|-------------|-----------|------------|------------|
|             |             |           |             |        3V3|   1   |   2   |+5.0V     |             |           |            |            |
|             |             |UART3_RX_M0|  I2C3_SDA_M0|   GPIO1_A0|   3   |   4   |+5.0V     |             |           |            |            |
|             |             |UART3_TX_M0|  I2C3_SCL_M0|   GPIO1_A1|   5   |   6   |GND       |             |UART2_TXD  |            |            |
|             |      PWM1_M1|           |  I2C2_SCL_M0|   GPIO0_B5|   7   |   8   |GPIO0_D1  |             |UART2_RXD  |            |            |
|             |             |           |             |        GND|   9   |   10  |GPIO0_D0  |             |           |            |            |
|             |     PWM14_M0|UART7_TX_M1|             |   GPIO3_C4|   11  |   12  |GPIO3_A3  |             |           |            |            |
|             |  PWM15_IR_M0|UART7_RX_M1|             |   GPIO3_C5|   13  |   14  |GND       |             |           |            |            |
|             |      PWM1_M0|   UART0_RX|             |   GPIO0_C0|   15  |   16  |GPIO0_B6  |I2C2_SDA_M0  |           |PWM2_M1     |            |
|             |             |           |             |        3V3|   17  |   18  |GPIO3_B2  |             |UART4_TX_M1|            |            |
| SPI3_MOSI_M1|  PWM15_IR_M1|           |             |   GPIO4_C3|   19  |   20  |GND       |             |           |            |            |
| SPI3_MISO_M1|     PWM12_M1|UART9_TX_M1|             |   GPIO4_C5|   21  |   22  |GPIO0_C1  |             |UART0_TX   |PWM2_M0     |            |
|  SPI3_CLK_M1|     PWM14_M1|           |             |   GPIO4_C2|   23  |   24  |GPIO4_C6  |             |UART9_RX_M1|PWM13_M1    |SPI3_CS0_M1 |
|             |             |           |             |        GND|   25  |   26  |GPIO4_D1  |             |           |            |SPI3_CS1_M1 |
|             |      PWM2_M1|           |     I2C1_SDA|   GPIO0_B4|   27  |   28  |GPIO0_B3  |I2C1_SCL     |           |PWM1_M1     |            |
|             |             |UART8_TX_M1|             |   GPIO2_D7|   29  |   30  |GND       |             |           |            |            |
|  SPI2_CLK_M1|             |UART8_RX_M1|             |   GPIO3_A0|   31  |   32  |GPIO3_C2  |             |UART5_TX_M1|            |            |
|             |             |UART5_RX_M1|             |   GPIO3_C3|   33  |   34  |GND       |             |           |            |            |
|             |             |           |             |   GPIO3_A4|   35  |   36  |GPIO3_A2  |             |           |            |            |
|             |             |           |             |SARADC_VIN5|   37  |   38  |GPIO3_A6  |             |           |            |            |
|             |             |           |             |        GND|   39  |   40  |GPIO3_A5  |             |           |            |            |

Supports
--------

You can find additional product support in the following channels:

- [Product Info](https://docs.radxa.com/en/rock3/rock3a)
- [Forums](https://forum.radxa.com/c/rock3)
- [Github](https://github.com/radxa)
