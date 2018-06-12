IEI TANK AIoT Dev. Kit  {#iei-tank}
================================
TANK AIoT Dev. Kit features rich I/O and dual PCIe by 16 slots with by 8 signal
for add-on card installation such as PoE (IPCEI-4POE) card or acceleration card
(Mustang-F100-A10 & Mustang-V100-RX8) to enhance function and performance for
various applications.

For the full specifications please visit the product page:

https://www.ieiworld.com/tank-aiot-development-kit

Interface notes
-----------------------
The MRAA platform file was developed using an IEI TANK-870-Q170 and may not
be compatible with other TANK systems. 6 serial ports and 8 digital pins are
available and can be controlled using MRAA on this system.

**UART**
 * 4 x RS-232 (2 x RJ-45, 2 x DB-9 w/2.5KV isolation protection)
 * 2 x RS-232/422/485 (DB-9)

**GPIO**
 * 8-bit digital I/O , 4-bit input / 4-bit output (DB9)

Pin Mapping
--------------------
The GPIO numbering in MRAA is explained in the table below. To use the pins
the *gpio_f7188x* Linux kernel module must be available and loaded.

| MRAA Pin | DB9 Pin      | Sysfs GPIO | Function        |
|----------|--------------|------------|-----------------|
| 0        | 1            | 4          | DIN0            |
| 1        | 2            | 0          | DOUT0           |
| 2        | 3            | 11         | DIN1            |
| 3        | 4            | 1          | DOUT1           |
| 4        | 5            | 12         | DIN2            |
| 5        | 6            | 2          | DOUT2           |
| 6        | 7            | 13         | DIN3            |
| 7        | 8            | 3          | DOUT3           |
| 8        | 9            |            | +5V             |
