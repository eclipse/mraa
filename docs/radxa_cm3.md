Radxa CM3 {#_Radxa}
=========

The Radxa CM3 is a System on Module (SoM) based on the Rockchip RK3566 System on Chip (SoC). CM3 integrates the Central Process Unit (CPU), Power Management Unit (PMU), DRAM memory, flash storage and wireless connectivity (WiFi 5 and BT 5.0) in a small form factor of just 55mm x 40mm. CM3 uses 3x 100P 0.4mm-pitch Board-to-Board connectors to export various features, and can be combined with the customer's baseboard to build complete products, thereby speeding up the research and development process. 
Currently, CM3 is compatible with Radxa CM3 IO Board and Raspberry Pi CM4 IO Board.

Interface notes
---------------

- UART2 is enabled as the default console.
- All UART ports support baud up to 1500000.

Pin Mapping
-----------

Radxa CM3 IO Board and Raspberry Pi CM4 IO Baseboard's 40-pin expansion header are compatible. The following pinout applies to both products:

|    Function3|   Function3|    Function2| Function1|  PIN  |  PIN  |  Function1|    Function2|   Function3|
|-------------|------------|-------------|----------|:------|------:|-----------|-------------|------------|
|             |            |             |       3V3|   1   |   2   |      +5.0V|             |            |
|      PWM2_M1|SPI0_MOSI_M0|  I2C2_SDA_M0|  GPIO0_B6|   3   |   4   |      +5.0V|             |            |
|      PWM1_M1| SPI0_CLK_M0|  I2C2_SCL_M0|  GPIO0_B5|   5   |   6   |        GND|             |            |
|             |            |             |  GPIO3_D5|   7   |   8   |   GPIO0_D1|  UART2_TX_M0|            |
|             |            |             |       GND|   9   |   10  |   GPIO0_D0|  UART2_RX_M0|            |
|             |            |      PWM0_M1|  GPIO0_C7|   11  |   12  |   GPIO3_C7|             |            |
|             |            |      PWM0_M0|  GPIO0_B7|   13  |   14  |        GND|             |            |
|             |            |         PWM4|  GPIO0_C3|   15  |   16  |   GPIO3_D4|             |            |
|             |            |             |     +3.3V|   17  |   18  |   GPIO3_D3|             |            |
|             | I2C4_SDA_M0| SPI3_MOSI_M0|  GPIO4_B2|   19  |   20  |        GND|             |            |
|             |            | SPI3_MISO_M0|  GPIO4_B0|   21  |   22  |   GPIO3_C6|             |            |
|             | I2C4_SCL_M0|  SPI3_CLK_M0|  GPIO4_B3|   23  |   24  |   GPIO4_A6|  SPI3_CS0_M0|            |
|             |            |             |       GND|   25  |   26  |   GPIO4_C5|  UART9_TX_M1|SPI3_MISO_M1|
|             |            |  I2C2_SDA_M1|  GPIO4_B4|   27  |   28  |   GPIO4_B5|  I2C2_SCL_M1|            |
|             |            |             |  GPIO4_B1|   29  |   30  |        GND|             |            |
|             |        PWM6| SPI0_MISO_M0|  GPIO0_C5|   31  |   32  |   GPIO4_C0|  UART5_TX_M1|            |
|             |     PWM7_IR|  SPI0_CS0_M0|  GPIO0_C6|   33  |   34  |        GND|             |            |
|             |            |             |  GPIO3_D0|   35  |   36  |   GPIO4_A7|  SPI3_CS1_M0|            |
|             |            |      PWM3_IR|  GPIO0_C2|   37  |   38  |   GPIO3_D2|             |            |
|             |            |             |       GND|   39  |   40  |   GPIO3_D1|             |            |

Supports
--------

You can find additional product support in the following channels:

- [Product Info](https://docs.radxa.com/en/compute-module/cm3)
- [Forums](https://forum.radxa.com/c/rock3)
- [Github](https://github.com/radxa)
