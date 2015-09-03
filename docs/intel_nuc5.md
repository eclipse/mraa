Intel NUC NUC5i5MYBE                             {#nuc5}
====================

Pinmuxing on the 5th generation Intel NUCs is done in the BIOS. This is only
tested on bios 0024+ (MYBDWi5v.86A). By default the custom solution header is
disabled, currently in Linux (as of 4.2). Both i2c buses are currently
supported.

The NUCs supported are the NUC5i5MYBE & NUC5i3MYBE which also come as the
NUC5i5MYHE and NUC5i3MYHE motherboards. It's possible that others expose the IO
in a very similar way so could be supported, get in touch if you have one!

In the BIOS you are required to enable the following:
Devices -> Onboard Devices - GPIO Lockdown
Select I2c under GPIO for the 12/13 14/15 pins

Interface notes
---------------

**I2C** Depending on your system you may need to load `i2c-dev`

Custom Solutions Header mapping
-------------------------------

The mapping is the same as the DE3815tykhe.

| MRAA Number | Physical Pin | Function     | Notes                |
|-------------|--------------|--------------|----------------------|
| 0           | 1            | 1.8V sby     |                      |
| 1           | 2            | GND          |                      |
| 2           | 3            | HDMI_CEC     |                      |
| 3           | 4            | DMIC_CLK     |                      |
| 4           | 5            | 3.3V sby     |                      |
| 5           | 6            | DMIC_DATA    |                      |
| 6           | 7            | Key (no pin) |                      |
| 7           | 8            | SMB_ALERT#   |                      |
| 8           | 9            | 5V sby (2A)  |                      |
| 9           | 10           | SCI_SMI_GPIO |                      |
| 10          | 11           | PWM[0]       |                      |
| 11          | 12           | PWM[1]       |                      |
| 12          | 13           | I2C0_CLK     | /dev/i2c-0 SCL       |
| 13          | 14           | I2C0_DATA    | /dev/i2c-0 SDA       |
| 14          | 15           | I2C1_CLK     | /dev/i2c-1 SCL       |
| 15          | 16           | I2C1_DATA    | /dev/i2c-1-SDA       |
| 16          | 17           | SMB_CLK      |                      |
| 17          | 18           | SMB_DATA     |                      |
