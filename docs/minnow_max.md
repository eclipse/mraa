Intel(R) Minnowboard Max               {#minnowmax}
========================
MinnowBoard MAX is an open hardware embedded board designed with the Intel(R)
Atom(TM) E38xx series SOC (Fromerly Bay Trail).

For product overview and faq see
http://www.minnowboard.org/faq-minnowboard-max/

For technical details see http://www.elinux.org/Minnowboard:MinnowMax

Supported Firmware
------------------
mraa has only been tested with 64 bit firmware version 0.73 or later.

SPI
---
For SPI support you need to load the low_speed_spidev kernel module and that
will create the /dev/spidev0.0 device node. Mraa only knows about this one SPI
bus and no other.

Interface notes
---------------
The low speed I/O connector supported as per table below.  This assumes default
BIOS settings, as they are not dynamcially detected If any changes are mode
(Device Manager -> System Setup -> South Cluster -> LPSS & CSS) them mraa calls
will not behave as expected.

Documentation shows i2c on bus #5, ACPI shows it on bus #6, but driver uses
bus #7.

| MRAA Number | Physical Pin  | Function   | Sysfs GPIO | Notes                |
|-------------|---------------|------------|------------|----------------------|
| 1           | 1             | GND        |            |                      |
| 2           | 2             | GND        |            |                      |
| 3           | 3             | 5v         |            |                      |
| 4           | 4             | 3.3v       |            |                      |
| 5           | 5             | SPI_CS     | 220        | SPI (via low_speed)  |
| 6           | 6             | UART1_TXD  | 225        | UART1                |
| 7           | 7             | SPI_MISO   | 221        | SPI (via low_speed)  |
| 8           | 8             | UART1_RXD  | 224        | UART1                |
| 9           | 9             | SPI_MOSI   | 222        | SPI (via low_speed)  |
| 10          | 10            | UART1_CTS  | 227        | GPIO                 |
| 11          | 11            | SPI_CLK    | 223        | SPI (via low_speed)  |
| 12          | 12            | UART1_RTS  | 226        | GPIO                 |
| 13          | 13            | I2C_SCL    | 243        | /dev/i2c-7           |
| 14          | 14            | I2S_CLK    | 216        | GPIO                 |
| 15          | 15            | I2C_SDA    | 242        | /dev/i2c-7           |
| 16          | 16            | I2S_FRM    | 217        | GPIO                 |
| 17          | 17            | UART2_TXD  | 229        | UART2                |
| 18          | 18            | I2S_DO     | 219        | GPIO                 |
| 19          | 19            | UART2_RXD  | 228        | UART2                |
| 20          | 20            | I2S_DI     | 218        | GPIO                 |
| 21          | 21            | GPIO_S5_0  |  82        | GPIO                 |
| 22          | 22            | PWM0       | 248        | PWM Chip 0 Channel 0 |
| 23          | 23            | GPIO_S5_1  |  83        | GPIO                 |
| 24          | 24            | PWM1       | 249        | PWM Chip 1 Channel 0 |
| 25          | 25            | S5_4       |  84        | GPIO                 |
| 26          | 26            | IBL_8254   | 208        | GPIO                 |
