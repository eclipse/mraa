Radxa ZERO {#_Radxa}
=====================

Radxa Zero is an ultra thin SBC in small form factor with powerful performance based on Amlogic S905Y2. It can run Android and selected Linux distributions.

Radxa Zero features a quad core 64-bit ARM processor, up to 4GB 32bit LPDDR4 memory, HDMI output at 4K@60, WiFi and BT connectivity, USB 3.0, and 40-pin GPIO header. Additionally, the power port can also be used for USB 2.0 OTG to connect more peripheral.

Radxa Zero comes in multiple configurations to suit your need. Please check Models & SKU for detail.

Pin Mapping (rev. 1.51)
-----------

| GPIO number |	Function4 |	Function3 | Function2 | Function1 | Pin# | Pin# | Function1 | Function2 | Function3 | Function4 | GPIO number |
|:-----:|:-----:|:-----:|:-----:|:-----:|-----:|:-----|:-----:|:-----:|:-----:|:-----:|:-----:|
|     |               |               |               | +3.3V        | **1**  | **2**  | +5.0V |     |               |               |               ||
| 490 |               |               | I2C_EE_M3_SDA | GPIOA_14 (*) | **3**  | **4**  | +5.0V       |               |               |               ||
| 491 |               |               | I2C_EE_M3_SCL | GPIOA_15 (*) | **5**  | **6**  | GND         |               |               |               ||
| 415 | I2C_AO_S0_SDA | UART_AO_B_RX | I2C_AO_M0_SDA  | GPIOAO_3     | **7**  | **8**  | GPIOAO_0    | UART_AO_A_TXD |               |               | 412 |
|     |               |               | GND           |              | **9**  | **10** | GPIOAO_1    | UART_AO_A_RXD |               |               | 413 | 
| 414 | I2C_AO_S0_SCL | UART_AO_B_TX  | I2C_AO_M0_SCL | GPIOAO_2     | **11** | **12** | GPIOX_9     | SPI_A_MISO    | TDMA_D0       |               | 501 | 
| 503 | TDMA_SCLK     | I2C_EE_M1_SCL |SPI_A_SCLK     | GPIOX_11     | **13** | **14** | GND         |               |               |               |
|     |               |               | SARADC_CH1	  |              | **15** | **16** | GPIOX_10    | SPI_A_SS0     | I2C_EE_M1_SDA | TDMA_FS       | 502 | 
|     |               |               | +3.3V         |              | **17** | **18** | GPIOX_8     | SPI_A_MOSI    | PWM_C         | TDMA_D1       | 500 | 
| 447 | SPI_B_MOSI    | UART_EE_C_RTS | GPIOH_4       |              | **19** | **20** | GND         |               |               |               |
| 448 | PWM_F         | SPI_B_MISO    | UART_EE_C_CTS | GPIOH_5      | **21** | **22** | GPIOC_7     | -             |               |               | 475 | 
| 450 | I2C_EE_M1_SCL | SPI_B_SCLK    | UART_EE_C_TX  | GPIOH_7      | **23** | **24** | GPIOH_6     | UART_EE_C_RX | SPI_B_SS0      | I2C_EE_M1_SDA | 449 | 
|     |               |               | GND           |              | **25** | **26** | SARADC_CH2  |               |               |               ||
| 415 | I2C_AO_S0_SDA | UART_AO_B_RX | I2C_AO_M0_SDA  | GPIOAO_3     | **27** | **28** | GPIOAO_2    | I2C_AO_M0_SCL | ART_AO_B_TX   | I2C_AO_S0_SCL | 414 | 
|     |               |               | NC            |              | **29** | **30** | GND         |               |               |               |
|     |               |               | NC            |              | **31** | **32** | GPIOAO_4    | PWMAO_C       |               |               | 416 | 
|     |               |               | NC            |              | **33** | **34** | GND         |               |               |               |
| 420 |               |               | UART_AO_B_TX  | GPIOAO_8     | **35** | **36** | GPIOH_8     | -             |               |               | 451 | 
| 421 |               |               | UART_AO_B_RX  | GPIOAO_9     | **37** | **38** | NC          |               |               |               | 
|     |               |               | GND           |              | **39** | **40** | GPIOAO_11   | PWMAO_A       |               |               | 423 | 

Supports
--------

You can find additional product support in the following channels:

- [Product Info](https://docs.radxa.com/en/zero/zero)
- [Wiki](https://wiki.radxa.com/Zero)
- [Forums](https://forum.radxa.com/c/zero)
- [Github](https://github.com/radxa)
