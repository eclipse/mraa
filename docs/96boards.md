96Boards Development Platform    {#_96boards}
=============================

96Boards is a range of hardware specifications created by Linaro to make the latest ARM-based processors available to developers at a reasonable cost. The specifications are open and define a standard board layout for SoC-agnostic (processor independent)  development platforms that can be used by software application, hardware device, kernel and other system software developers. Boards produced to the 96Boards specifications are suitable for rapid prototyping, hobbyist projects or incorporation into new systems for a wide range of applications including desktop and laptop computing, the digital home, digital signage, point of sale (POS), high-end audio, robotics and drones, artificial intelligence, virtual reality, IoT and industrial control. 

Standardized expansion buses for peripheral I/O have led to a wide range of compatible add-on mezzanine boards  that will work across a variety of 96Boards products. Users have access to a wide range of boards with different features at various price points. In addition, some SoC vendors have announced long term availability of the SoC to encourage their use in products with long life cycles.

Board Support
-------------

- [Bubblegum-96](http://www.96boards.org/product/bubblegum-96/)
- [DragonBoard 410c](http://www.96boards.org/product/dragonboard410c/)
- [DragonBoard 820c](http://www.96boards.org/product/dragonboard820c/)
- [HiKey](http://www.96boards.org/product/hikey/)
- [HiKey960](http://www.96boards.org/product/hikey960/)
- [Rock960](http://www.96boards.org/product/rock960/)
- [Ultra96](https://www.96boards.org/product/ultra96/)

Interface notes
---------------

- **GPIO**: Pin 28 / GPIO-F is a Multi-Purpose pin
- **UART**: UART1 can be used for serial access

Pin Mapping
-----------

Pin mapping table shows signals pertaining to 40-pin low speed expansion header based on [96Boards Consumer Edition Specification](https://github.com/96boards/documentation/blob/master/Specifications/96Boards-CE-Specification.pdf).

|  96Boards Signals |  PIN  |  PIN  |  96Boards Signals  |
|:------------------|:------|------:|:-------------------|
|     GND           |   1   |   2   |    GND             |
|     UART0_CTS     |   3   |   4   |    PWR_BTN_N       |
|     UART0_TxD     |   5   |   6   |    RST_BTN_N       |
|     UART0_RxD     |   7   |   8   |    SPI0_SCLK       |
|     UART1_RTS     |   9   |   10  |    SPI0_DIN        |
|     UART1_TxD     |   11  |   12  |    SPI0_CS         |
|     UART1_RxD     |   13  |   14  |    SPI0_DOUT       |
|     I2C0_SCL      |   15  |   16  |    PCM_FS          |
|     I2C0_SDA      |   17  |   18  |    PCM_CLK         |
|     I2C1_SCL      |   19  |   20  |    PCM_DO          |
|     I2C1_SDA      |   21  |   22  |    PCM_DI          |
|     GPIO-A        |   23  |   24  |    GPIO-B          |
|     GPIO-C        |   25  |   26  |    GPIO-D          |
|     GPIO-E        |   27  |   28  |    GPIO-F          |
|     GPIO-G        |   29  |   30  |    GPIO-H          |
|     GPIO-I        |   31  |   32  |    GPIO-J          |
|     GPIO-K        |   33  |   34  |    GPIO-L          |
|     +1V8          |   35  |   36  |    SYS_DCIN        |
|     +5V           |   37  |   38  |    SYC_DCIN        |
|     GND           |   39  |   40  |    GND             |

Resources
---------

The following links will take you to addition 96Boards resources

- [Website](http://www.96boards.org/)
- [Forums](https://discuss.96boards.org/)
- [Documentation Repo](https://github.com/96boards/documentation)
