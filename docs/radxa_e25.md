Radxa E25 {#_Radxa}
====================

Radxa E25 is a Rockchip RK3568 based SBC(Single Board Computer) by Radxa. It can run Android or Linux. Radxa E25 features a four core ARM processor, 64bit dual channel 3200Mb/s LPDDR4, HDMI up to 4K60p, MIPI DSI, MIPI CSI, 3.5mm combo audio jack, Wi-Fi 6, Bluetooth 5.0, USB, GbE LAN, and 40-pin color expansion header. Radxa E25 is powered by the USB Type-C port, and supports 5V input only. The recommended power adapter is 5V/3A without SSD, or 5V/5A with SSD.

Interface notes
---------------

- All UART ports support baud up to 1500000.
- Radxa E25 v1.3 or earlier with 10-pin header are **NOT** supported. Only v1.4 or later with 26-pin header are supported.

Pin Mapping
-----------

Radxa E25 has a 40-pin expansion header. Each pin is distinguished by the color.

|    Function5|    Function4|  Function3|  Function2|  Function1|  PIN  |  PIN  |  Function1|    Function2|  Function3|   Function4|   Function5|
|-------------|-------------|-----------|-----------|-----------|:------|------:|-----------|-------------|-----------|------------|------------|
|             |             |           |           |  3V3      |   1   |   2   |    +5.0V  |             |           |            |            |
|             |             |I2C3_SDA_M0|UART3_RX_M0|GPIO1_A0   |   3   |   4   |    +5.0V  |             |           |            |            |
|             |             |I2C3_SCL_M0|UART3_TX_M0|GPIO1_A1   |   5   |   6   |     GND   |             |           |            |            |
|             |PWM12_M0     |           |UART3_TX_M1|GPIO3_B7   |   7   |   8   |   GPIO3_C2|  UART5_TX_M1|           |            |SPI1_MISO_M1|
|             |             |           |           |  GND      |   9   |   10  |   GPIO3_C3|  UART5_RX_M1|           |            | SPI1_CLK_M1|
|             |PWM14_M0     |           |UART7_TX_M1|GPIO3_C4   |   11  |   12  |   GPIO3_A3|             |           |            |            |
|             |             |           |UART7_RX_M1|GPIO3_C5   |   13  |   14  |     GND   |             |           |            |            |
|SPI1_MOSI_M1 |             |           |           |GPIO3_C1   |   15  |   16  |   GPIO2_D2|             |           |            | SPI0_CSO_M1|
|SPI1_CSO_M1  |             |           |           |GPIO3_A1   |   17  |   18  |   GPIO0_C6|             |           |     PWM7_IR| SPI0_CS0_M0|
|SPI0_MOSI_M1 |             |           |           |GPIO2_D1   |   19  |   20  |    GND    |             |           |            |            |
|SPI0_MISO_M1 |             |           |           |GPIO2_D0   |   21  |   22  |SARADC_VIN5|             |           |            |            |
|SPI0_CLK_M1  |             |           |           |GPIO2_D3   |   23  |   24  |   GPIO4_C6|             |           |    PWM13_M1|            |
|             |             |           |           |  GND      |   25  |   26  |   GPIO3_C0|  UART3_RX_M1|           |    PWM13_M0|            |

Supports
--------

You can find additional product support in the following channels:

- [Product Info](https://docs.radxa.com/en/rock3/e25)
- [Forums](https://forum.radxa.com/c/rock3)
- [Github](https://github.com/radxa)
