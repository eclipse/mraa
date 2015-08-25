Intel Edison                            {#edison}
=============

Intel(R) Edison is a dual-core Silvermont Atom(TM) clocked at 500MHz. The
Edison also features 4GB of storage, 1GB ram and on-board WiFi and Bluetooth.

Currently supported boards:
- Intel Arduino board
- Intel breakout board

UART
----
On both the Arduino board and the breakout board, The available UART interface is on /dev/ttyMFD1

Intel Arduino board
-------------------
The Edison used with the Arduino board has the following limitations
in libmraa:

- I2C is exposed on i2c-6, therefore you must use bus 6 and not bus 0
- PWM available on default swizzler positions. (3,5,6,9)
- SPI exposed is also used for the ADC. Try not to use your own CS.
- Max SPI speed is 25Mhz/4 ~6.25Mhz
- SPI PM can sometimes do weird things you can disable it with:
  `echo on > /sys/devices/pci0000\:00/0000\:00\:07.1/power/control`
- ADC kernel module will return 16bit number but the ADC itself only has an
  accuracy of maximum 12bits and in MRAA it's limited to 10bits by default.
  Use `mraa_aio_set_bit(12)` to switch to the maximum resolution mode.
  This ADC is only included on the Arduino board.
- AIO pins are treated as 0-5 in `mraa_aio_init()` but as 14-19 for everything
  else. Therefore use `mraa_gpio_init(14)` to use A0 as a GPIO
- Arduino pin 7 can sometimes negatively impact the WiFi capability, if using
  WiFi avoid using this pin

Because of the way IO is setup with the tristate on the Arduino breakout board
IO will be flipped as it is setup. It's recommended to setup IO pins &
direction before using them in a `setup()` method or similar. It's impossible on
this platform to avoid some GPIOs flipping on setup.

Intel(R) breakout board
-----------------------

- Both I2C buses are available 1 & 6
- IO on the miniboard is 1.8V
- Requesting GPIO 4 will break your FTDI UART console, so bear in mind when trying to use it

Please see the following table on how the physical pins map to mraa pin numbers

| MRAA Number | Physical Pin | Edison Pin    | Notes                   | Pinmode0 | Pinmode1   | Pinmode2 |
|-------------|--------------|---------------|-------------------------|----------|------------|----------|
| 0           | J17-1        | GP182         |                         | GPIO-182 | PWM2       |          |
| 1           | J17-2        | NC            | Nothing from mraa       |          |            |          |
| 2           | J17-3        | NC            | Nothing from mraa       |          |            |          |
| 3           | J17-4        | VIN           | Nothing from mraa       |          |            |          |
| 4           | J17-5        | GP135         |                         | GPIO-135 | UART       |          |
| 5           | J17-6        | RCVR_MODE     | Nothing from mraa       |          |            |          |
| 6           | J17-7        | GP27          |                         | GPIO-27  | I2C-6-SCL  |          |
| 7           | J17-8        | GP20          |                         | GPIO-20  | I2C-1-SDA  |          |
| 8           | J17-9        | GP28          |                         | GPIO-28  | I2C-6-SDA  |          |
| 9           | J17-10       | GP111         |                         | GPIO-111 | SPI-5-CS1  |          |
| 10          | J17-11       | GP109         |                         | GPIO-109 | SPI-5-SCK  |          |
| 11          | J17-12       | GP115         |                         | GPIO-115 | SPI-5-MOSI |          |
| 12          | J17-13       | OSC_CLK_OUT_0 | Nothing from mraa/check |          |            |          |
| 13          | J17-14       | GP128         |                         | GPIO-128 | UART-1-CTS |          |
| 14          | J18-1        | GP13          |                         | GPIO-13  | PWM1       |          |
| 15          | J18-2        | GP165         |                         | GPIO-165 |            |          |
| 16          | J18-3        | GPI_PWRBTN_N  | Nothing from mraa       |          |            |          |
| 17          | J18-4        | MSIC_SLP_CLK2 | Nothing from mraa       |          |            |          |
| 18          | J18-5        | V_VBAT_BKUP   | Nothing from mraa       |          |            |          |
| 19          | J18-6        | GP19          |                         | GPIO-19  | I2C-1-SCL  |          |
| 20          | J18-7        | GP12          | PWM0                    | GPIO-12  | PWM0       |          |
| 21          | J18-8        | GP183         | PWM3                    | GPIO-183 | PWM3       |          |
| 22          | J18-9        | NC            | Nothing from mraa       |          |            |          |
| 23          | J18-10       | GP110         |                         | GPIO-110 | SPI-5-CS0  |          |
| 24          | J18-11       | GP114         |                         | GPIO-114 | SPI-5-MISO |          |
| 25          | J18-12       | GP129         |                         | GPIO-129 | UART-1-RTS |          |
| 26          | J18-13       | GP130         |                         | GPIO-130 | UART-1-RX  |          |
| 27          | J18-14       | FW_RCVR       | Nothing from mraa       |          |            |          |
| 28          | J19-1        | NC            | Nothing from mraa       |          |            |          |
| 29          | J19-2        | V_V1P80       | Nothing from mraa       |          |            |          |
| 30          | J19-3        | GND           | Nothing from mraa       |          |            |          |
| 31          | J19-4        | GP44          |                         | GPIO-44  |            |          |
| 32          | J19-5        | GP46          |                         | GPIO-46  |            |          |
| 33          | J19-6        | GP48          |                         | GPIO-48  |            |          |
| 34          | J19-7        | RESET_OUT     | Nothing from mraa       |          |            |          |
| 35          | J19-8        | GP131         |                         | GPIO-131 | UART-1-TX  |          |
| 36          | J19-9        | GP14          |                         | GPIO-14  |            |          |
| 37          | J19-10       | GP40          |                         | GPIO-40  | SSP2_CLK   |          |
| 38          | J19-11       | GP43          |                         | GPIO-43  | SSP2_TXD   |          |
| 39          | J19-12       | GP77          |                         | GPIO-77  | SD         |          |
| 40          | J19-13       | GP82          |                         | GPIO-82  | SD         |          |
| 41          | J19-14       | GP83          |                         | GPIO-83  | SD         |          |
| 42          | J20-1        | V_VSYS        | Nothing from mraa       |          |            |          |
| 43          | J20-2        | V_V3P30       | Nothing from mraa       |          |            |          |
| 44          | J20-3        | GP134         |                         |          |            |          |
| 45          | J20-4        | GP45          |                         | GPIO-45  |            |          |
| 46          | J20-5        | GP47          |                         | GPIO-47  |            |          |
| 47          | J20-6        | GP49          |                         | GPIO-49  |            |          |
| 48          | J20-7        | GP15          |                         | GPIO-15  |            |          |
| 49          | J20-8        | GP84          |                         | GPIO-84  | SD         |          |
| 50          | J20-9        | GP42          |                         | GPIO-42  | SSP2_RXD   |          |
| 51          | J20-10       | GP41          |                         | GPIO-41  | SSP2_FS    |          |
| 52          | J20-11       | GP78          |                         | GPIO-78  | SD         |          |
| 53          | J20-12       | GP79          |                         | GPIO-79  | SD         |          |
| 54          | J20-13       | GP80          |                         | GPIO-80  | SD         |          |
| 55          | J20-14       | GP81          |                         | GPIO-81  | SD         |          |
