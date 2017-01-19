phyBOARD-Wega    {#phyboard-wega}
=============

The phyBOARD-Wega for phyCORE-AM335x is a low-cost, feature-rich software
development platform supporting the Texas Instruments AM335x microcontroller.

Moreover, due to the numerous standard interfaces the phyBOARD-Wega AM335x can
serve as bedrock for your application. At the core of the phyBOARD-Wega is the
PCL-051/phyCORE-AM335x System on Module (SOM) in a direct solder form factor,
containing the processor, DRAM, NAND Flash, power regulation, supervision,
transceivers, and other core functions required to support the AM335x
processor. Surrounding the SOM is the PBA-CD-02/phyBOARD-Wega carrier board,
adding power input, buttons, connectors, signal breakout, and Ethernet
connectivity amongst other things.

There are (for example) some expansion boards available for the phyBOARD-Wega:
- Power Module (PEB-POW-01)
- Evaluation Board (PEB-EVAL-01)
- HDMI Adapter (PEB-AV-01)

For further information and instruction please visit:
www.phytec.de/produkt/system-on-modules/single-board-computer/phyboard-wega


Interface notes
---------------

**SPI**
Spi0 can be used over pins on X69 connector. There are no more spi-devices.
Independent of the given index in mraa_phyboard_spi_init_pre() function,
it will be always initialized spi0.

**I2C**
/* TODO */

**UART**
Uart0 can be used over pins on X69 connector or serial port on Evaluation Board.
The other Uarts not have been tested yet. This is a small TODO for future.


Pin Assignment of X69 Expansion Connector
-----------------------------------------

| Physical Pin | Signal Name         | Type | SL    | Description             |
|--------------|---------------------|------|-------|-------------------------|
| 1            | VCC3V3              | OUT  | 3.3 V | 3.3V power supply       |
| 2            | VCC5V               | OUT  | 5.0 V | 5V power supply         |
| 3            | VDIG1_1P8V          | OUT  | 1.8 V | 1.8V power supply       |
|              |                     |      |       |      (max. 300mA)       |
| 4            | GND                 | -    | -     | Ground                  |
| 5            | X_SPIO_CS0          | OUT  | 3.3 V | SPI 0 chip select 0     |
| 6            | X_SPIO_MOSI         | OUT  | 3.3 V | SPI 0 master output /   |
|              |                     |      |       |       slave input       |
| 7            | X_SPIO_MISO         | IN   | 3.3 V | SPI 0 master input /    |
|              |                     |      |       |       slave output      |
| 8            | X_SPIO_CLK          | OUT  | 3.3 V | SPI 0 clock output      |
| 9            | GND                 | -    | -     | Ground                  |
| 10           | X_UART0_RXD         | IN   | 3.3 V | UART 0 receive data     |
|              |                     |      |       |      (std. debug iface) |
| 11           | X_I2C0_SDA          | I/O  | 3.3 V | I2C0 Data               |
| 12           | X_UART0_TXD         | OUT  | 3.3 V | UART 0 transmit data    |
|              |                     |      |       |      (std. debug iface) |
| 13           | X_I2C0_SCL          | I/O  | 3.3 V | I2C0 Clock              |
| 14           | GND                 | -    | -     | Ground                  |
| 15           | X_JTAG_TMS          | IN   | 3.3 V | JTAG Chain Test         |
|              |                     |      |       |      Mode Select signal |
| 16           | X_nJTAG_TRST        | IN   | 3.3 V | JTAG Chain Test Reset   |
| 17           | X_JTAG_TDI          | IN   | 3.3 V | JTAG Chain Test         |
|              |                     |      |       |      Data Input         |
| 18           | X_JTAG_TDO          | OUT  | 3.3 V | JTAG Chain Test         |
|              |                     |      |       |      Data Output        |
| 19           | GND                 | -    | -     | Ground                  |
| 20           | X_JTAG_TCK          | IN   | 3.3 V | JTAG Chain Test         |
|              |                     |      |       |      Clock signal       |
| 21           | X_USB_DP_EXP        | I/O  | 3.3 V | USB data plus           |
|              |                     |      |       |      (for USB0 or USB1) |
| 22           | X_USB_DM_EXP        | I/O  | 3.3 V | USB data minus          |
|              |                     |      |       |      (for USB0 or USB1) |
| 23           | nRESET_OUT          | OUT  | 3.3 V | Reset                   |
| 24           | GND                 | -    | -     | Ground                  |
| 25           | X_MMC2_CMD          | I/O  | 3.3 V | MMC command             |
| 26           | X_MMC2_DATO         | I/O  | 3.3 V | MMC data 0              |
| 27           | X_MMC2_CLK          | I/O  | 3.3 V | MMC clock               |
| 28           | X_MMC2_DAT1         | I/O  | 3.3 V | MMC data 1              |
| 29           | GND                 | -    | -     | Ground                  |
| 30           | X_MMC2_DAT2         | I/O  | 3.3 V | MMC data 2              |
| 31           | X_UART2_RX_GPIO3_9  | I/O  | 3.3 V | UART 2 receive data     |
|              |                     |      |       | or GPIO3_9              |
| 32           | X_MMC2_DAT3         | I/O  | 3.3 V | MMC data 3              |
| 33           | X_UART2_TX_GPIO3_10 | I/O  | 3.3 V | UART 2 transmit data    |
|              |                     |      |       | or GPIO3_10             |
| 34           | GND                 | -    | -     | Ground                  |
| 35           | X_UART3_RX_GPIO2_18 | I/O  | 3.3 V | UART 3 receive data     |
|              |                     |      |       | or GPIO2_18             |
| 36           | X_UART3_TX_GPIO2_19 | I/O  | 3.3 V | UART 3 transmit data    |
|              |                     |      |       |        or GPIO2_19      |
| 37           | X_INTR1_GPIO0_20    | I/O  | 3.3 V | Interrupt 1 or GPIO0_20 |
| 38           | X_GPIO0_7           | I/O  | 3.3 V | GPIO0_7                 |
| 39           | X_AM335_EXT_WAKEUP  | IN   | 3.3 V | External wakeup         |
| 40           | X_INT_RTCn          | OUT  | 3.3 V | Interrupt from the RTC  |
| 41           | GND                 | -    | -     | Ground                  |
| 42           | X_GPIO3_7_nPMOD_PW  | I/O  | 3.3 V | GPIO3_7; Caution! Also  |
|              | RFAIL               |      |       | connected to power fail |
|              |                     |      |       | signal through R415.    |
| 43           | nRESET_IN           | IN   | 3.3 V | Push-button reset       |
| 44           | X_GPIO1_31          | I/O  | 3.3 V | GPIO1_31                |
| 45           | X_AM335_NMIn        | IN   | 3.3 V | AM335x                  |
|              |                     |      |       | non-maskable interrupt  |
| 46           | GND                 | -    | -     | Ground                  |
| 47           | X_AIN4              | IN   | 1.8 V | Analog input 4          |
| 48           | X_AIN5              | IN   | 1.8 V | Analog input 5          |
| 49           | X_AIN6              | IN   | 1.8 V | Analog input 6          |
| 50           | X_AIN7              | IN   | 1.8 V | Analog input 7          |
| 51           | GND                 | -    | -     | Ground                  |
| 52           | X_GPIO_CKSYNC       | I/O  | 3.3 V | GPIO Clock              |
|              |                     |      |       | Synchronization         |
| 53           | X_USB_ID_EXP        | IN   | 1.8 V | USB port identification |
|              |                     |      |       |      (for USB0 or USB1) |
| 54           | USB_VBUS_EXP        | OUT  | 5.0 V | USB bus voltage         |
|              |                     |      |       |      (for USB0 or USB1) |
| 55           | X_USB1_CE           | OUT  | 3.3 V | USB 1 charger enable    |
| 56           | GND                 | -    | -     | Ground                  |
| 57           | VCC_BL              | OUT  | NS    | Backlight power supply  |
| 58           | X_BP_POWER          | IN   | 5.0 V | Power On for Power      |
|              |                     |      |       | Management IC for AM335x|
| 59           | GND                 | -    | -     | Ground                  |
| 60           | VCC5V_IN            | IN   | 5.0 V | 5 V input supply voltage|


Pin Assignment of X70 A/V Connector
-----------------------------------

| Physical Pin | Signal Name         | Type | SL    | Description             |
|--------------|---------------------|------|-------|-------------------------|
| 1            | GND                 | -    | -     | Ground                  |
| 2            | X_LCD_D21           | OUT  | 3.3 V | LCD D21                 |
| 3            | X_LCD_D18           | OUT  | 3.3 V | LCD D18                 |
| 4            | X_LCD_D16           | OUT  | 3.3 V | LCD D16                 |
| 5            | X_LCD_D0            | OUT  | 3.3 V | LCD D0                  |
| 6            | GND                 | -    | -     | Ground                  |
| 7            | X_LCD_D1            | OUT  | 3.3 V | LCD D1                  |
| 8            | X_LCD_D2            | OUT  | 3.3 V | LCD D2                  |
| 9            | X_LCD_D3            | OUT  | 3.3 V | LCD D3                  |
| 10           | X_LCD_D4            | OUT  | 3.3 V | LCD D4                  |
| 11           | GND                 | -    | -     | Ground                  |
| 12           | X_LCD_D22           | OUT  | 3.3 V | LCD D22                 |
| 13           | X_LCD_D19           | OUT  | 3.3 V | LCD D19                 |
| 14           | X_LCD_D5            | OUT  | 3.3 V | LCD D5                  |
| 15           | X_LCD_D6            | OUT  | 3.3 V | LCD D6                  |
| 16           | GND                 | -    | -     | Ground                  |
| 17           | X_LCD_D7            | OUT  | 3.3 V | LCD D7                  |
| 18           | X_LCD_D8            | OUT  | 3.3 V | LCD D8                  |
| 19           | X_LCD_D9            | OUT  | 3.3 V | LCD D9                  |
| 20           | X_LCD_D10           | OUT  | 3.3 V | LCD D10                 |
| 21           | GND                 | -    | -     | Ground                  |
| 22           | X_LCD_D23           | OUT  | 3.3 V | LCD D23                 |
| 23           | X_LCD_D20           | OUT  | 3.3 V | LCD D20                 |
| 24           | X_LCD_D17           | OUT  | 3.3 V | LCD D17                 |
| 25           | X_LCD_D11           | OUT  | 3.3 V | LCD D11                 |
| 26           | GND                 | -    | -     | Ground                  |
| 27           | X_LCD_D12           | OUT  | 3.3 V | LCD D12                 |
| 28           | X_LCD_D13           | OUT  | 3.3 V | LCD D13                 |
| 29           | X_LCD_D14           | OUT  | 3.3 V | LCD D14                 |
| 30           | X_LCD_D15           | OUT  | 3.3 V | LCD D15                 |
| 31           | GND                 | -    | -     | Ground                  |
| 32           | X_LCD_PCLK          | OUT  | 3.3 V | LCD Pixel Clock         |
| 33           | X_LCD_BIAS_EN       | OUT  | 3.3 V | LCD BIAS                |
| 34           | X_LCD_HSYNC         | OUT  | 3.3 V | LCD Horizontal          |
|              |                     |      |       |     Synchronization     |
| 35           | X_LCD_VSYNC         | OUT  | 3.3 V | LCD Vertical            |
|              |                     |      |       |     Synchronisation     |
| 36           | GND                 | -    | -     | Ground                  |
| 37           | GND                 | -    | -     | Ground                  |
| 38           | X_PWM1_OUT          | OUT  | 3.3 V | Pulse Width Modulation  |
| 39           | VCC_BL              | OUT  | NS    | Backlight power supply  |
| 40           | VCC5V               | OUT  | 5.0 V | 5 V power supply        |


Pin Assignment of X71 A/V Connector
-----------------------------------

| Physical Pin | Signal Name         | Type | SL    | Description             |
|--------------|---------------------|------|-------|-------------------------|
| 1            | X_I2S_CLK           | I/O  | 3.3 V | I2S Clock               |
| 2            | X_I2S_FRM           | I/O  | 3.3 V | I2S Frame               |
| 3            | X_I2S_ADC           | I/O  | 3.3 V | I2S Analog-Digital      |
|              |                     |      |       |  converter (microphone) |
| 4            | X_I2S_DAC           | I/O  | 3.3 V | I2S Digital-Analog      |
|              |                     |      |       |  converter (speaker)    |
| 5            | X_AV_INT_GPIO1_30   | I/O  | 3.3 V | A/V interrupt; GPIO1_30 |
| 6            | nUSB1_OC_GPIO3_19 or| I/O  | 3.3 V | GPIO3_19 or McASP0      |
|          | X_MCASP0_AHCLKX_GPIO3_21|      |       | high frequency clock    |
| 7            | GND                 | -    | -     | Ground                  |
| 8            | nRESET_OUT          | OUT  | 3.3 V | Reset                   |
| 9            | TS_X+               | IN   | 1.8 V | Touch X+                |
| 10           | TS_X-               | IN   | 1.8 V | Touch X-                |
| 11           | TS_Y+               | IN   | 1.8 V | Touch Y+                |
| 12           | TS_Y-               | IN   | 1.8 V | Touch Y-                |
| 13           | VCC3V3              | OUT  | 3.3 V | 3.3 V power supply      |
| 14           | GND                 | -    | -     | Ground                  |
| 15           | X_I2C0_SCL          | I/O  | 3.3 V | I2C Clock               |
| 16           | X_I2C0_SDA          | I/O  | 3.3 V | I2C Data                |

Jumper J77 connects either signal X_MCASP0_AHCLKX_GPIO3_21 or signal
nUSB1_OC_GPIO3_19 to pin 6 of X71.
The following table shows the available configurations:

A/V Jumper configuration J77
----------------------------

| J77 | Description              |
|-----|--------------------------|
| 1+2 | X_MCASP0_AHCLKX_GPIO3_21 |
| 2+3 | nUSB1_OC_GPIO3_19        |

Caution: If J77 is set to 2+3 , J78 also has to be set to 2+3 !


GPIO - Pin-mapping (with installed expansion board)
---------------------------------------------------

| Physical Pin | Pin-Name      | Pin-Map / Sysfs GPIO | Def. usage | Connector|
|--------------|---------------|----------------------|------------|----------|
| 31           | X_GPIO3_9     | 105                  | OUT  LED3  | X69      |
| 33           | X_GPIO3_10    | 106                  | IN   S3    | X69      |
| 35           | X_GPIO2_18    |  82 (busy)           | OUT  LED1  | X69      |
| 36           | X_GPIO2_19    |  83                  | OUT  LED2  | X69      |
| 37           | X_GPIO0_20    |  20 (busy)           | IN   S1    | X69      |
| 38           | X_GPIO0_7     |   7                  | IN   S2    | X69      |
| 42           | X_GPIO3_7     | 103                  | IN         | X69      |
| 44           | X_GPIO1_31    |  63                  | IN         | X69      |
|--------------|---------------|----------------------|------------|----------|
| 5 (105)      | X_GPIO1_30    |  62                  | IN         | X71      |
| 6 (106)      | X_GPIO3_19 or | 115                  | IN         | X71      |
|              | X_GPIO3_21    | 117                  | IN         | X71      |

Info: (busy) means, that it is used by kernel driver!
