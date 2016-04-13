Raspberry Pi    {#rasppi}
============

The Raspberry Pi is based on the Broadcom BCM2835 system on a chip, which
includes an ARM1176JZF-S 700 MHz processor, VideoCore IV GPU and was originally
shipped with 256 megabytes of RAM, later upgraded (Model B & Model B+) to 512
MB. The Raspberry Pi 2 is based on an armv7 CPU but shares a number of
similarities with the original.

Revision Support
----------------
Raspberry Pi A/B/A+/B+
Raspberry Pi 2 B

Interface notes
---------------

**PWM** Whilst the Raspberry Pi is meant to have 1 PWM channel this is currently not supported.

**GPIO MMAP** GPIO memory mapping may be used. For example, [UPM library](https://github.com/intel-iot-devkit/upm) uses GPIO memory maping for control Chip Select `CS` pin on SPI bus. 
MRRA can use two models for GPIO memory mapping:

* MRAA use `/dev/gpiomem` and users from group `gpio` can read-write, without needing root privileges, but new bcm2835-based kernel modules needed
* MRAA use `/dev/mem` and root privileges required. For security reasons, run programs with root privilegies not good idea.

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
You may need to change the permissions and ownership of `/dev/gpiomem` if they have not been correctly set up.
```
sudo chown root:gpio /dev/gpiomem
sudo chmod g+rw /dev/gpiomem
```

The user (default `pi`) needs to be in the `gpio` group: `sudo usermod -aG gpio pi`.


Also, if needed, you can add new spi module manually `sudo modprobe spi_bcm2835`.
