Radxa ROCK Pi E {#_Radxa}
====================

Radxa ROCK Pi E is a Rockchip RK3288 based single board computer by Radxa. It can run Android or Linux.

Interface notes
---------------

- UART2 is enabled as the default console.
- All UART ports support baud up to 1500000.

Pin Mapping
-----------

Radxa ROCK Pi E has a 40-pin expansion header. Each pin is distinguished by the color.

| Function5| Function4|  Function3|    Function2| Function1|  PIN  |  PIN  | Function1|    Function2|  Function3|   Function4|   Function5|
|----------|----------|-----------|-------------|----------|:------|------:|----------|-------------|-----------|------------|------------|
|          |          |           |             |       3V3|   1   |   2   |     +5.0V|             |           |            |            |
|          |          |           |    UART1_TXD|  GPIO3_A4|   3   |   4   |     +5.0V|             |           |            |            |
|          |          |           |    UART1_RXD|  GPIO3_A6|   5   |   6   |       GND|             |           |            |            |
|          |          |           |             |  GPIO1_D4|   7   |   8   |  GPIO2_A0|  UART2_TX_M1|           |            |            |
|          |          |           |             |       GND|   9   |   10  |  GPIO2_A1|  UART2_RX_M1|           |            |            |
|          |          |           |             |  GPIO2_A2|   11  |   12  |  GPIO2_C2|             |           |            |            |
|          |          |           |             |  GPIO2_A3|   13  |   14  |       GND|             |           |            |            |
|          |          |           |             |  GPIO0_D3|   15  |   16  |   USB20DM|             |           |            |            |
|          |          |           |             |     +3.3V|   17  |   18  |   USB20DP|             |           |            |            |
|          |          |SPI_TXD_M2 |             |  GPIO3_A1|   19  |   20  |       GND|             |           |            |            |
|          |          |SPI_RXD_M2 |             |  GPIO3_A2|   21  |   22  |SARADC_IN1|             |           |            |            |
|          |          |SPI_CLK_M2 |             |  GPIO3_A0|   23  |   24  |  GPIO3_B0|             |SPI_CSN0_M2|            |            |
|          |          |           |             |       GND|   25  |   26  |  GPIO2_B4|             |           |            |            |
|PWM0      |I2C1_SDA  |           |             |  GPIO2_A4|   27  |   28  |  GPIO2_A5|             |           |    I2C1_SCL|        PWM1|
|          |          |           |             |  GPIO2_C4|   29  |   30  |       GND|             |           |            |            |
|          |          |           |             |  GPIO2_C5|   31  |   32  |  GPIO2_C0|             |           |            |            |
|PWM2      |          |           |             |  GPIO2_A6|   33  |   34  |       GND|             |           |            |            |
|          |          |           |             |  GPIO2_C1|   35  |   36  |  GPIO2_B7|             |           |            |            |
|          |          |           |             |  GPIO2_C6|   37  |   38  |  GPIO2_C3|             |           |            |            |
|          |          |           |             |       GND|   39  |   40  |  GPIO2_C7|             |           |            |            |

Supports
--------

You can find additional product support in the following channels:

- [Product Info](https://docs.radxa.com/rockpi/rockpie)
- [Forums](https://forum.radxa.com/c/rockpie)
- [Github](https://github.com/radxa)