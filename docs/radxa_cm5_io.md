Radxa CM5 IO {#_Radxa}
============

The Radxa CM5 is a System on Module (SoM) based on the Rockchip RK3588s System on Chip (SoC). CM5 is compatible with Radxa CM5 IO Board. It can run android or some Linux distributions. Radxa CM5 IO features an eight core ARM processor, 64bit dual channel 3200Mb/s LPDDR4, up to 8Kp60 HDMI, MIPI DSI, MIPI CSI, 3.5mm jack with mic, 802.11 ac WIFI, Bluetooth 5.0, USB Port, GbE LAN, 40-pin color expansion header, RTC. Also, Radxa CM5 IO supports USB PD and QC powering.

Interface notes
---------------

- UART2 is enabled as the default console.
- All UART ports support baud up to 1500000.

Pin Mapping
-----------

Radxa CM5 IO has a 40-pin expansion header. Each pin is distinguished by color.

|Function4   |Function3   |Function2   |Function1  |       |  PIN  |  PIN    |   Function1|  Function2|   Function3|
|------------|------------|------------|-----------|:------|------:|---------|------------|-----------|------------|
|            |            |            |+3.3V      |   1   |   2   |    +5.0V|            |           |            |
|            |            |I2C7_SDA_M2 |GPIO3_D3   |   3   |   4   |    +5.0V|            |           |            |
|            |            |I2C7_SCL_M2 |GPIO3_D2   |   5   |   6   |      GND|            |           |            |
|            |            |I2C5_SDA_M2 |GPIO4_A7   |   7   |   8   | GPIO0_B5| UART2_TX_M0|I2C1_SCL_M0|            |
|            |            |            |GND        |   9   |  10   | GPIO0_B6| UART2_RX_M0|I2C1_SDA_M0|            |
|            |UART3_RX_M2 |I2C5_SCL_M2 |GPIO4_A6   |  11   |  12   | GPIO0_C2|            |           |            |
|            |UART3_TX_M2 |I2C3_SDA_M2 |GPIO4_A5   |  13   |  14   |      GND|            |           |            |
|            |            |I2C3_SCL_M2 |GPIO4_A4   |  15   |  16   | GPIO1_C4| PWM11_IR_M2|           |            |
|            |            |            |+3.3V      |  17   |  18   | GPIO1_D5|            |           |            |
|            |            |SPI0_MOSI_M1|GPIO4_A1   |  19   |  20   |      GND|            |           |            |
|            |            |SPI0_MISO_M1|GPIO4_A0   |  21   |  22   | GPIO1_B1|            |           |            |
|            |            |SPI0_CLK_M1 |GPIO4_A2   |  23   |  24   | GPIO4_B2| SPI0_CS0_M1|   PWM14_M1|            |
|            |            |            |GND        |  25   |  26   | GPIO3_B7|            |           |            |
|            |            |            |+3.3V      |  27   |  28   |    +3.3V|            |           |            |
|            |            |PWM7_IR_M0  |GPIO0_D0   |  29   |  30   |      GND|            |           |            |
|            |            |PWM6_M0     |GPIO0_C7   |  31   |  32   | GPIO1_B7|    PWM13_M2|           |            |
|            |            |            |GPIO1_C1   |  33   |  34   |      GND|            |           |            |
|            |            |            |GPIO3_C0   |  35   |  36   | GPIO1_C6| PWM15_IR_M2|           |            |
|            |            |            |SARADC_VIN4|  37   |  38   | GPIO1_D2|     PWM0_M1|           |            |
|            |            |            |GND        |  39   |  40   | GPIO0_D3|            |           |            |

Resources
---------

You can find additional product support in the following channels:

- [Product Info](https://docs.radxa.com/compute-module/cm5)
- [Forums](https://forum.radxa.com/c/rock5)
- [Github](https://github.com/radxa)
