Orange Pi Prime Single Board Computer    {#_orange_pi_prime}
=============================

Orange Pi Prime is a Allwinner H5 based SBC(Single Board Computer) by Orange Pi. It can run android or some Linux distributions. Orange Pi Prime features a quad core ARM processor, 2GB DDR3, HDMI, CSI, 3.5mm jack with CVBS video out, onboard mic, 802.11 b/g/n WIFI, Bluetooth 4.0, USB Port, Ethernet, 40-pin expansion header. Also, Orange Pi Prime supports USB OTG and has barrel jack for power.

Board Support
-------------

- [Orange Pi Prime](http://www.orangepi.org/orangepiwiki/index.php/Orange_Pi_Prime)

Interface notes
---------------

- On Armbian, use armbian-config to enable UART, I2C, PWM and SPI on expansion header.

Pin Mapping
-----------

Orange Pi Prime has a 40-pin expansion header.

|Additional Function |  Primary Function|  PIN  |  PIN  |  Primary Function  | Additional Function |
|--------------------|------------------|:------|------:|--------------------|---------------------|
|                    |      +3.3V       |   1   |   2   |    +5.0V           |                     |
|           I2C0_SDA |      PA12        |   3   |   4   |    +5.0V           |                     |
|           I2C0_SCK |      PA11        |   5   |   6   |    GND             |                     |
|                    |      PA6         |   7   |   8   |    PC5             |                     |
|                    |      GND         |   9   |   10  |    PC6             |                     |
|           UART2_RX |      PA1         |   11  |   12  |    PD14            |                     |
|           UART2_TX |      PA0         |   13  |   14  |    GND             |                     |
|          UART2_CTS |      PA3         |   15  |   16  |    PC4             |                     |
|                    |      +3.3V       |   17  |   18  |    PC7             |                     |
|          SPI1_MOSI |      PA15        |   19  |   20  |    GND             |                     |
|          SPI1_MISO |      PA16        |   21  |   22  |    PA2             | UART2_RTS           |
|           SPI1_CLK |      PA14        |   23  |   24  |    PA13            | SPI1_CS             |
|                    |      GND         |   25  |   26  |    PC8             |                     |
|           I2C1_SDA |      PA19        |   27  |   28  |    PA18            | I2C1_SCK            |
|                    |      PA7         |   29  |   30  |    GND             |                     |
|                    |      PA8         |   31  |   32  |    PC9             |                     |
|                    |      PA9         |   33  |   34  |    GND             |                     |
|                    |      PA10        |   35  |   36  |    PC10            |                     |
|                    |      PD11        |   37  |   38  |    PC11            |                     |
|                    |      GND         |   39  |   40  |    PC12            |                     |

Resources
---------

The following links will take you to additional Orange Pi Prime resources

- [Armbian for Orange Pi Prime](https://www.armbian.com/orange-pi-prime/)
- [Armbian forum for Orange Pi Prime](https://forum.armbian.com/forum/136-orange-pi-prime/)
- [Armbian Github Repo](https://github.com/armbian)
