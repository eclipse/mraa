Rock Pi S Single Board Computer    {#_rockpi}
=============================

ROCK Pi S is a Rockchip RK3308 based SBC(Single Board Computer) by Radxa. It equips a 64bits quad core processor, USB, ethernet, wireless connectivity and voice detection engine at the size of 1.7 inch, make it perfect for IoT and voice applications. ROCK Pi S comes in two ram sizes 256MB or 512MB DDR3, and uses uSD card for OS and storage. Optionally, ROCK Pi S can provide on board storage version with 1Gb/2Gb/4Gb/8Gb NAND flash.

ROCK Pi S board comes with different versions. When you get the board, you need to know the hardware version like 'ROCK PI S V12', which is printed in the top side of the board.

Board Support
-------------

- [ROCK Pi S](https://wiki.radxa.com/RockpiS)

Interface notes
---------------

- **UART**: ttyS0, ttyS2, and ttyS4
- UART0 is enabled as U-boot and Linux serial console by default. Check RockpiS/dev/serial-console to use. Check /Device-tree-overlays to disable serial console on UART0.
- Function marked with color orange is the default function of this pin.
- Except Pins for power supply, all pins are laid out directly to Soc RK3308.
- RK3308 have two IO voltages, 1.8V/3.3V. For ROCK Pi S, the voltage level of GPIOs showed in the tables above are 3.3V and tolerance of those are 3.63V. For hardware V11 and V12, an ADC input is included (ADC_IN0). This ADC has an input voltage range of 0-1.8V.
- The pins, PIN#2 and PIN#4, can supply power of 5V. The maximum current that can output depends on the power adapter.
- The pins, PIN#1 and PIN#17, can supply power of 3.3V. The maximum current that can output is 200mA.

Pin Mapping
-----------

ROCK Pi 4 has two 26-pin expansion headers. Each pin is distinguished by color.
The function of each pin depends on the board version. The common functions between boards V11 to V13 on header 1 are shown below. GPIO names vary between board versions (V13 names are used below)), see /RockpiS/hardware/gpio.

| Additional FunctionS |  Primary Function|  PIN  |  PIN  |  Primary Function  | Additional Functions |
|----------------------|------------------|:------|------:|--------------------|----------------------|
|          |           |      +3.3V       |   1   |   2   |    +5.0V           |           |          |
|          |           |      I2C1_SDA    |   3   |   4   |    +5.0V           |           |          |
|          |           |      I2C1_SCL    |   5   |   6   |    GND             |           |          |
|          |           |      GPIO2_A4    |   7   |   8   |    UART0_TXD       |           |          |
|          |           |      GND         |   9   |   10  |    UART0_RXD       |           |          |
|          |           |      PWM2        |   11  |   12  |    GPIO2_A5        |           |          |
|          |           |      PWM3        |   13  |   14  |    GND             |           |          |
|          |           |      GPIO0_C1    |   15  |   16  |    GPIO2_B2        |           |          |
|          |           |      +3.3V       |   17  |   18  |    GPIO2_B1        |           |          |
|          | UART2_TXD |      SPI2_TXD    |   19  |   20  |    GND             |           |          |
|          | UART2_RXD |      SPI2_RXD    |   21  |   22  |    GPIO2_A7        |           |          |
| I2C0_SDA | UART1_RXD |      SPI2_CLK    |   23  |   24  |    SPI2_CSn        | UART1_TXD | I2C0_SCL |
|          |           |      GND         |   25  |   26  |    ADC_IN0         |           |          |

Resources
---------

The following links will take you to additional Rock Pi S resources

- [Armbian for Rock Pi S](https://www.armbian.com/rockpi-s/)
- [Forums](https://forum.radxa.com/c/rockpiS)
- [Github Repo](https://github.com/radxa)
