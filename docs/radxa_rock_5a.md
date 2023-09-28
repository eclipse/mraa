Radxa ROCK 5 Model A {#_Radxa}
====================

Radxa ROCK 5A is a Rockchip RK3588s based SBC(Single Board Computer) by Radxa. It can run android or some Linux distributions. Radxa ROCK 5A features a eight core ARM processor, 64bit dual channel 3200Mb/s LPDDR4, up to 8Kp60 HDMI, MIPI DSI, MIPI CSI, 3.5mm jack with mic, 802.11 ac WIFI, Bluetooth 5.0, USB Port, GbE LAN, 40-pin color expansion header, RTC. Also, Radxa ROCK 5A supports USB PD and QC powering.

Interface notes
---------------

- UART2 is enabled as the default console.
- All UART ports support baud up to 1500000.

Pin Mapping
-----------

Radxa ROCK 5A has a 40-pin expansion header. Each pin is distinguished by color.

|Function4   |Function3   |Function2   |Function1  |       |  PIN  |  PIN    |   Function1|  Function2|   Function3|
|------------|------------|------------|-----------|:------|------:|---------|------------|-----------|------------|
|            |            |            |+3.3V      |   1   |   2   |    +5.0V|            |           |            |
|            |PWM15_IR_M3 |I2C8_SDA_M2 |GPIO1_D7   |   3   |   4   |    +5.0V| UART2_TX_M0|I2C1_SCL_M0|            |
|            |PWM14_M2    |I2C8_SCL_M2 |GPIO1_D6   |   5   |   6   |      GND| UART2_RX_M0|I2C1_SDA_M0|            |
|            |UART4_TX_M2 |SPI0_CLK_M2 |GPIO1_B3   |   7   |   8   | GPIO0_B5|SPI0_MOSI_M1|           |            |
|            |            |            |GND        |   9   |   10  | GPIO0_B6|            |           |            |
|            |PWM15_IR_M1 |I2C7_SDA_M3 |GPIO4_B3   |   11  |   12  | GPIO4_A1|            |           |            |
|PWM14_M1    |SPI0_CS0_M1 |I2C7_SCL_M3 |GPIO4_B2   |   13  |   14  |      GND|            |           |            |
|            |PWM11_IR_M1 |UART9_TX_M1 |GPIO4_B4   |   15  |   16  | GPIO1_A5|SPI2_MOSI_M0|           |            |
|            |            |            |+3.3       |   17  |   18  | GPIO1_B0| SPI2_CS1_M0|           |            |
|UART6_TX_M1 |SPI4_MOSI_M2|I2C2_SCL_M4 |GPIO1_A1   |   19  |   20  |      GND|            |           |            |
|UART6_RX_M1 |SPI4_MISO_M2|I2C2_SDA_M4 |GPIO1_A0   |   21  |   22  | GPIO1_B5| SPI0_CS1_M2|UART7_TX_M2|            |
|PWM0_M2     |SPI4_CLK_M2 |I2C4_SDA_M3 |GPIO1_A2   |   23  |   24  | GPIO1_A3| I2C4_SCL_M3|SPI4_CS0_M2|     PWM1_M2|
|            |            |            |     GND   |   25  |   26  | GPIO1_A4|SPI2_MISO_M0|           |            |
|SPI0_MISO_M0|I2C6_SDA_M0 |PWM6_M0     |GPIO0_C7   |   27  |   28  | GPIO0_D0|  PWM7_IR_M0|I2C6_SCL_M0|SPI3_MISO_M2|
|            |UART4_RX_M2 |SPI0_MOSI_M |GPIO1_B2   |   29  |   30  |      GND|            |           |            |
|            |            |SPI0_MISO_M2|GPIO1_B1   |   31  |   32  | GPIO4_B0| I2C6_SDA_M3|SPI2_CS1_M1| UART8_TX_M0|
|            |UART7_RX_M2 |SPI0_CS0_M2 |GPIO1_B4   |   33  |   34  |      GND|            |           |            |
|            |            |SPI0_MISO_M1|GPIO4_A0   |   35  |   36  | GPIO4_A2| SPI0_CLK_M1|           |            |
|            |            |            |SARADC_VIN2|   37  |   38  | GPIO4_A5| I2C3_SDA_M2|UART3_TX_M2|            |
|            |            |            |GND        |   39  |   40  | GPIO4_B1| I2C6_SCL_M3|SPI0_CS1_M1| UART8_RX_M0|

Resources
---------

You can find additional product support in the following channels:

- [Product Info](https://docs.radxa.com/en/rock5/rock5a)
- [Forums](https://forum.radxa.com/c/rock5)
- [Github](https://github.com/radxa)
