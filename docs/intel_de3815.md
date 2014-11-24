Intel(R) NUC DE3815tykhe                            {#de3815}
=============

The DE3815 NUC Kit is a single core Atom(TM) clocked at 1.46GHz.
http://www.intel.com/content/www/us/en/nuc/nuc-kit-de3815tykhe.html

Interface notes
---------------
**GPIO** Not yet implemented.

**PWM** Functionality for the PWM is provided by the `pwm_lpss` module `CONFIG_PWM_LPSS`

**I2C** Depending on your system you may need to load `i2c-dev`

Custom Solutions Header mapping
-------------------------------
Based on: http://downloadmirror.intel.com/23745/eng/DE3815TYBE_TechProdSpec06.pdf

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
| 10          | 11           | PWM[0]       | PWM Chip 0 Channel 0 |
| 11          | 12           | PWM[1]       | PWM Chip 0 Channel 1 |
| 12          | 13           | I2C0_CLK     | /dev/i2c-0 SCL       |
| 13          | 14           | I2C0_DATA    | /dev/i2c-0 SDA       |
| 14          | 15           | I2C1_CLK     | /dev/i2c-1 SCL       |
| 15          | 16           | I2C1_DATA    | /dev/i2c-1-SDA       |
| 16          | 17           | SMB_CLK      |                      |
| 17          | 18           | SMB_DATA     |                      |
