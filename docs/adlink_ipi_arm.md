ADLINK IPi-SMARC ARM
============

Based on Rockchip PX30 with Quad-core ARM Cortex-A35 CPU  

Up to 4GB DDR3L at 1066/1333 MHz,  32 GB eMMC  

4x USB 2.0, 1x USB 2.0 OTG port, 2x 10/100M Ethernet Ports, DSI to HDMI converter board, RTC and a 40-pin I/O header




## Pin Mapping 

| Linux GPIO (/sys/class/gpio) | Function  | MRAA number | MRAA number | Function  | Linux GPIO (/sys/class/gpio) |
| :--------------------------: | :-------: | :---------: | :---------: | :-------: | :--------------------------: |
|                              |    3V3    |      1      |      2      |    5V     |                              |
|                              | I2C_0 SDA |      3      |      4      |    5V     |                              |
|                              | I2C_0 SCL |      5      |      6      |    GND    |                              |
|             116              |   GPIO    |      7      |      8      |  UART TX  |                              |
|                              |    GND    |      9      |     10      |  UART RX  |                              |
|             118              |   GPIO    |     11      |     12      |   GPIO    |             117              |
|             107              |   GPIO    |     13      |     14      |    GND    |                              |
|             104              |   GPIO    |     15      |     16      |   GPIO    |             109              |
|                              |    3V3    |     17      |     18      |   GPIO    |             121              |
|                              | SPI MOSI  |     19      |     20      |    GND    |                              |
|                              | SPI MISO  |     21      |     22      |   GPIO    |             122              |
|                              | SPI SCLK  |     23      |     24      |  SPI CS0  |                              |
|                              |    GND    |     25      |     26      |      N/A  |                              |
|                              | I2C_1 SDA |     27      |     28      | I2C_1 SCL |                              |
|             496              | GPIO/PWM  |     29      |     30      |    GND    |                              |
|             497              | GPIO/PWM  |     31      |     32      | GPIO/PWM  |             498              |
|             499              | GPIO/PWM  |     33      |     34      |    GND    |                              |
|             500              | GPIO/PWM  |     35      |     36      | GPIO/PWM  |             501              |
|             502              | GPIO/PWM  |     37      |     38      | GPIO/PWM  |             503              |
|                              |    GND    |     39      |     40      | GPIO/PWM  |             504              |

**Note**: N/A: Funciton is not supported
