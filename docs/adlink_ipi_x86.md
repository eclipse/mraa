ADLINK IPi-SMARC x86
============

* ADLINK IPi-SMARC x86

  Based on Intel(R) Atom® E3900 Series(codename: Apollo Lake) with Movidius chip  

  up to 8GB RAM, 64GB eMMC 

  2x USB 2.0, 2x USB 3.0, 1x USB 2.0 OTG port, 1x Gigabit Ethernet port, HDMI display, RTC and a 40-pin I/O header





## Pin Mapping 


| Linux GPIO (/sys/class/gpio) | Function  | MRAA number | MRAA number | Function  | Linux GPIO (/sys/class/gpio) |
| :--------------------------: | :-------: | :---------: | :---------: | :-------: | :--------------------------: |
|                              |    3V3    |      1      |      2      |    5V     |                              |
|                              | I2C_0 SDA |      3      |      4      |    5V     |                              |
|                              | I2C_0 SCL |      5      |      6      |    GND    |                              |
|             255              |   GPIO    |      7      |      8      |  UART TX  |                              |
|                              |    GND    |      9      |     10      |  UART RX  |                              |
|             256              |   GPIO    |     11      |     12      |   GPIO    |             257              |
|             258              |   GPIO    |     13      |     14      |    GND    |                              |
|             259              |   GPIO    |     15      |     16      |   GPIO    |             260              |
|                              |    3V3    |     17      |     18      |   GPIO    |             261              |
|                              |    n/a    |     19      |     20      |    GND    |                              |
|                              |    n/a    |     21      |     22      |   GPIO    |             262              |
|                              |    n/a    |     23      |     24      |    n/a    |                              |
|                              |    GND    |     25      |     26      |    n/a    |                              |
|                              | I2C_1 SDA |     27      |     28      | I2C_1 SCL |                              |
|             234              | GPIO/PWM  |     29      |     30      |    GND    |                              |
|             235              | GPIO/PWM  |     31      |     32      | GPIO/PWM  |             236              |
|             237              | GPIO/PWM  |     33      |     34      |    GND    |                              |
|             238              | GPIO/PWM  |     35      |     36      | GPIO/PWM  |             239              |
|             240              | GPIO/PWM  |     37      |     38      | GPIO/PWM  |             241              |
|                              |    GND    |     39      |     40      | GPIO/PWM  |             242              |

**Note:** 

* Before installing MRAA on Ubuntu 18.04, please follow up the below instructions to insert the following drivers first and configure I2C devices:

    ```
     $ sudo modprobe i2c_i801
     $ sudo modprobe gpio-pca953x
     $ echo "pca9535 0x20" > /sys/bus/i2c/devices/i2c-13/new_device
     $ echo "sx1509q 0x3e" > /sys/bus/i2c/devices/i2c-1/new_device
    ```

* Not support SPI interface



