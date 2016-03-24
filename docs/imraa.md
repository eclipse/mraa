# Building mraa with imraa	{#buildingimraa}
Imraa allows seamless communication between the Arduino101 and your mraa projects.

## Build dependencies
Not all these are required but if you're unsure of what you're doing this is
what you'll need:
* The build dependencies found here: [Building mraa](../master/docs/building.md)
* [DFU-UTILS-CROSS](https://github.com/arduino/dfu-utils-cross)
* [Json-c](https://github.com/json-c/json-c)
* Libudev: For debian-like distros this can be installed as such ```sudo apt-get install libudev-dev```

## Basic build steps
This will build mraa with imraa for the local default path. To build mraa for the system add `-DCMAKE_INSTALL_PREFIX=/usr` to the cmake command before running it.

```bash
mkdir build
cd build
cmake -DIMRAA=ON -DFIRMATA=ON ..
make
```

After you've built mraa you can install using `make install`.

## Running Imraa
Before being able to use mraa to program the Arduino101 you will need to run imraa to flash the sketch file onto the board. The sketch file that imraa will look for is located here: [Arduino101 Sketch](http://iotdk.intel.com/misc/ConfigurableFirmataCurieImu.ino.bin). Once downloaded the file needs to be renamed and moved to the where mraa was installed. This is dependant on whether you installed locally or for the system. The example below is for a locally installed copy.
```bash
$ mv ConfigurableFirmataCurieImu.ino.bin firmata101.ino.bin
$ mv firmata101.ino.bin /usr/local/share/mraa
```

You will also need to move the imraa config so that imraa can find it.
```bash
$ mv mraa/imraa/imraa.conf /etc/
```

To flash the board run imraa and it will create the lockfile and flash the Arduino101.
```
$ imraa
```

After this is successful you will be able to use mraa like normal, in the background mraa will handle talking to the arduino101.
