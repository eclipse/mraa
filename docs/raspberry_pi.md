Raspberry Pi    {#rasppi}
============

The Raspberry Pi is based on the Broadcom BCM2835 system on a chip, which
includes an ARM1176JZF-S 700 MHz processor, VideoCore IV GPU and was originally
shipped with 256 megabytes of RAM, later upgraded (Model B & Model B+) to 512
MB. The Raspberry Pi 2 is based on an armv7 CPU but shares a number of
similarities with the original. The Raspberry Pi 3 is the third generation Raspberry Pi. 
It replaced the Raspberry Pi 2 Model B in February 2016.

Revision Support
----------------
Raspberry Pi A/B/A+/B+
Raspberry Pi 2 B
Raspberry Pi 3 B

Interface notes
---------------

**PWM** Whilst the Raspberry Pi is meant to have 1 PWM channel this is currently not supported.

**GPIO MMAP** (memory mapping) may be used. For example, 
[UPM library](https://github.com/intel-iot-devkit/upm) uses GPIO memory maping for 
control Chip Select `CS` pin on SPI bus. MRRA can use two models for GPIO memory mapping:

* MRAA use `/dev/gpiomem` and users from group `gpio` can read-write, 
without needing root privileges, but new bcm2835-based kernel modules needed
* MRAA use `/dev/mem` and root privileges required. For security reasons, 
run programs with root privilegies not good idea.

Normal user (usually, `pi`), can have full read-write privilegies for access to `/dev/gpiomem`. 
For enabling `/dev/gpiomem` in you system, you must use bcm2835-based kernel modules, instead old bcm2708. 

For [Raspbian Jessie](https://www.raspberrypi.org/downloads/raspbian/)


upgrade system:

```
sudo apt-get update
sudo apt-get upgrade
```

then run `sudo raspi-config` and: 

* Go to Advanced options and disabling "SPI"
* Go to Advanced options and enabling "Device Tree"
* Click "Finish" and reboot

Check `/dev/gpiomem` for exist: `ls -l /dev/gpiomem`. 
You may need to change the permissions and ownership of `/dev/gpiomem` 
if they have not been correctly set up:
```
sudo chown root:gpio /dev/gpiomem
sudo chmod g+rw /dev/gpiomem
```

The user (default `pi`) needs to be in the `gpio` group: `sudo usermod -aG gpio pi`.


Also, if needed, you can add new spi module manually `sudo modprobe spi_bcm2835`.

Pin Mapping
-----------

This pin mapping refers to the first generation 'B' model Pi but gives an idea
as to what the values are from mraa. Note that there is an emum to use wiringPi
style numbers.

| MRAA Number | Physical Pin | Function |
|-------------|--------------|----------|
| 1           | P1-01        | 3V3 VCC  |
| 2           | P1-02        | 5V VCC   |
| 3           | P1-03        | I2C SDA  |
| 4           | P1-04        | 5V VCC   |
| 5           | P1-05        | I2C SCL  |
| 6           | P1-06        | GND      |
| 7           | P1-07        | GPIO(4)  |
| 8           | P1-08        | UART TX  |
| 9           | P1-09        | GND      |
| 10          | P1-10        | UART RX  |
| 11          | P1-11        | GPIO(17) |
| 12          | P1-12        | GPIO(18) |
| 13          | P1-13        | GPIO(21) |
| 14          | P1-14        | GND      |
| 15          | P1-15        | GPIO(22) |
| 16          | P1-16        | GPIO(23) |
| 17          | P1-17        | 3V3 VCC  |
| 18          | P1-18        | GPIO(24) |
| 19          | P1-19        | SPI MOSI |
| 20          | P1-20        | GND      |
| 21          | P1-21        | SPI MISO |
| 22          | P1-22        | GPIO(25) |
| 23          | P1-23        | SPI SCL  |
| 24          | P1-24        | SPI CS0  |
| 25          | P1-25        | GND      |
| 26          | P1-26        | SPI CS1  |


The following pin mapping applies to the new Raspberry Pi model 3, 2 and B+. 
Note that GPIO(21) is now GPIO(27)

| MRAA Number | Physical Pin | Function |
|-------------|--------------|----------|
| 1           | P1-01        | 3V3 VCC  |
| 2           | P1-02        | 5V VCC   |
| 3           | P1-03        | I2C SDA  |
| 4           | P1-04        | 5V VCC   |
| 5           | P1-05        | I2C SCL  |
| 6           | P1-06        | GND      |
| 7           | P1-07        | GPIO(4)  |
| 8           | P1-08        | UART TX  |
| 9           | P1-09        | GND      |
| 10          | P1-10        | UART RX  |
| 11          | P1-11        | GPIO(17) |
| 12          | P1-12        | GPIO(18) |
| 13          | P1-13        | GPIO(27) |
| 14          | P1-14        | GND      |
| 15          | P1-15        | GPIO(22) |
| 16          | P1-16        | GPIO(23) |
| 17          | P1-17        | 3V3 VCC  |
| 18          | P1-18        | GPIO(24) |
| 19          | P1-19        | SPI MOSI |
| 20          | P1-20        | GND      |
| 21          | P1-21        | SPI MISO |
| 22          | P1-22        | GPIO(25) |
| 23          | P1-23        | SPI SCL  |
| 24          | P1-24        | SPI CS0  |
| 25          | P1-25        | GND      |
| 26          | P1-26        | SPI CS1  |
| 27          | P1-27        | I2C SDA1 |
| 28          | P1-28        | I2C SCL1 |
| 29          | P1-29        | GPIO(5)  |
| 30          | P1-30        | GND      |
| 31          | P1-31        | GPIO(6)  |
| 32          | P1-32        | GPIO(12) |
| 33          | P1-33        | GPIO(13) |
| 34          | P1-34        | GND      |
| 35          | P1-35        | SPI MISO1|
| 36          | P1-36        | GPIO(16) |
| 37          | P1-37        | GPIO(26) |
| 38          | P1-38        | SPI MOSI1|
| 39          | P1-39        | GND      |
| 40          | P1-40        | SPI SCL1 |
