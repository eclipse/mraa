Terasic DE10-Nano   {#de10-nano}
=================

The DE10-Nano Development Kit presents a robust hardware design platform built around the Intel
System-on-Chip (SoC) FPGA, which combines the latest dual-core Cortex-A9 embedded cores with
industry-leading programmable logic for ultimate design flexibility. Users can now leverage the
power of tremendous re-configurability paired with a high-performance, low-power processor system.
Altera’s SoC integrates an ARM-based hard processor system (HPS) consisting of processor,
peripherals and memory interfaces tied seamlessly with the FPGA fabric using a high-bandwidth
interconnect backbone. The DE10-Nano development board is equipped with high-speed DDR3 memory,
analog to digital capabilities, Ethernet networking, and much more that promise many exciting
applications.

Board Support
-------------

* Terasic DE10-Nano
* Terasic DE0-Nano-SoC

Protocol Notes
---------------

* **GPIO** Fully supported through sysfs (both FPGA + HPC). Mmap not yet supported. Static map.
* **PWM** Currently not supported.
* **I2C** Fully supported with i2cdev. Default bus 2 is exposed on Arduino header, 3 buses total.
* **SPI** Supported with spidev on Arduino header.
* **UART** Working, default bus is on Arduino header. Console also accessible as `ttyS0` in raw mode.
* **ADC** Currently not supported.

No muxes available for multi-function pins yet (e.g. you won't be able to use D0 & D1 on Arduino
header as GPIOs).

Pin Mapping
-----------

**Arduino Header**

| MRAA Number | Arduino Name | Board Pin | Function |
|-------------|--------------|-----------|----------|
| 0           | D0           | JP3-8     | UART RX  |
| 1           | D1           | JP3-7     | UART TX  |
| 2           | D2           | JP3-6     | GPIO     |
| 3           | D3           | JP3-5     | GPIO     |
| 4           | D4           | JP3-4     | GPIO     |
| 5           | D5           | JP3-3     | GPIO     |
| 6           | D6           | JP3-2     | GPIO     |
| 7           | D7           | JP3-1     | GPIO     |
| 8           | D8           | JP2-10    | GPIO     |
| 9           | D9           | JP2-9     | GPIO     |
| 10          | D10          | JP2-8     | SPI SS   |
| 11          | D11          | JP2-7     | SPI MOSI |
| 12          | D12          | JP2-6     | SPI MISO |
| 13          | D13          | JP2-5     | SPI SCK  |
| 14          | GND          | JP2-4     | -        |
| 15          | AREF         | JP2-3     | -        |
| 16          | SDA          | JP2-2     | I2C SDA  |
| 17          | SCL          | JP2-1     | I2C SCL  |
| 18          | AREF         | JP5-1     | -        |
| 19          | IOREF        | JP5-2     | -        |
| 20          | RESET        | JP5-3     | -        |
| 21          | 3V3          | JP5-4     | -        |
| 22          | 5V           | JP5-5     | -        |
| 23          | GND          | JP5-6     | -        |
| 24          | GND          | JP5-7     | -        |
| 25          | VIN          | JP5-8     | -        |
| 26          | A0           | JP6-1     | AIO      |
| 27          | A1           | JP6-2     | AIO      |
| 28          | A2           | JP6-3     | AIO      |
| 29          | A3           | JP6-4     | AIO      |
| 30          | A4           | JP6-5     | AIO      |
| 31          | A5           | JP6-6     | AIO      |

**GPIO0 (JP1) Side Header**

| MRAA Number | Board Pin | Function |
|-------------|-----------|----------|
| 32          | JP1-1     | GPIO     |
| ...         | ...       | GPIO     |
| 41          | JP1-10    | GPIO     |
| 42          | JP1-11    | 5V       |
| 43          | JP1-12    | GND      |
| 44          | JP1-13    | GPIO     |
| ...         | ...       | GPIO     |
| 59          | JP1-28    | GPIO     |
| 60          | JP1-29    | 3V3      |
| 61          | JP1-30    | GND      |
| 62          | JP1-31    | GPIO     |
| ...         | ...       | GPIO     |
| 71          | JP1-40    | GPIO     |

**GPIO1 (JP7) Side Header**

| MRAA Number | Board Pin | Function |
|-------------|-----------|----------|
| 72          | JP7-1     | GPIO     |
| ...         | ...       | GPIO     |
| 81          | JP7-10    | GPIO     |
| 82          | JP7-11    | 5V       |
| 83          | JP7-12    | GND      |
| 84          | JP7-13    | GPIO     |
| ...         | ...       | GPIO     |
| 99          | JP7-28    | GPIO     |
| 100         | JP7-29    | 3V3      |
| 101         | JP7-30    | GND      |
| 102         | JP7-31    | GPIO     |
| ...         | ...       | GPIO     |
| 111         | JP7-40    | GPIO     |

**Switches, Buttons, LEDs**

These are already in use by kernel drivers with the default image, except for KEY0 and KEY1.
To use them with MRAA remove them from the device tree.

To enable the switches temporarily (until reboot):

```sh
echo ff204000.gpio >/sys/bus/platform/drivers/altera_gpio/unbind
echo ff204000.gpio >/sys/bus/platform/drivers/altera_gpio/bind
```

And for LEDs:

```sh
echo ff203000.gpio >/sys/bus/platform/drivers/altera_gpio/unbind
echo ff203000.gpio >/sys/bus/platform/drivers/altera_gpio/bind
```

| MRAA Number | Board Pin | Function |
|-------------|-----------|----------|
| 112         | SW0       | GPIO     |
| 113         | SW1       | GPIO     |
| 114         | SW2       | GPIO     |
| 115         | SW3       | GPIO     |
| 116         | LED0      | GPIO     |
| 117         | LED1      | GPIO     |
| 118         | LED2      | GPIO     |
| 119         | LED3      | GPIO     |
| 120         | LED4      | GPIO     |
| 121         | LED5      | GPIO     |
| 122         | LED6      | GPIO     |
| 123         | LED7      | GPIO     |
| 124         | HPS_LED   | GPIO     |
| 125         | HPS_KEY   | GPIO     |
| 126         | KEY0      | GPIO     |
| 127         | KEY1      | GPIO     |

**Built-in ADXL345**

Exposed on I2C bus 0. To use you will have to unbind the device driver first:

```sh
echo 0-0053 > /sys/bus/i2c/drivers/adxl34x/unbind
```
