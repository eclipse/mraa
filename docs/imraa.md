# Building mraa with imraa	{#buildingimraa}
Imraa allows seamless communication between the Arduino101 and your mraa
projects. Imraa will flash an arduino101 and allow automatic loading of the
MRAA_GENERIC_FIRMATA subplatform to the first process that calls imraa_init
(done through mraa_init automatically)

## Build dependencies
To build imraa, you'll need to build mraa with -DIMRAA=ON and all the normal
dependencies for build mraa, see [Building mraa](./building.md).
You'll also need the following:
* [dfu-utils-cross](https://github.com/arduino/dfu-utils-cross) or dfu 0.8 (0.9
  does not work well with the 101). Precompiled binaries can be found on
  [here](https://github.com/01org/intel-arduino-tools)
* [json-c](https://github.com/json-c/json-c): likely called libjson-c-dev or
  json-c-devel
* libudev: likely called libudev-dev or libudev-devel

## Basic build steps
This will build mraa with imraa for the local default path. To build mraa for
the system add `-DCMAKE_INSTALL_PREFIX=/usr` to the cmake command before
running it.

```bash
mkdir build
cd build
cmake -DIMRAA=ON -DFIRMATA=ON ..
make
```

After you've built mraa you can install using `make install`.

## Running Imraa
Before being able to use mraa to program the Arduino101 you will need to run
imraa to flash the sketch file onto the board. The sketch file that imraa will
look for is located here: [Arduino101 Sketch](http://iotdk.intel.com/misc/ConfigurableFirmataCurieImu.ino.bin).
Once downloaded the file needs to be renamed and moved to the where mraa was
installed. This is dependant on whether you installed locally or for the
system. The example below is for a locally installed copy. Note this sketch is
using ConfigurableFirmata + the [CurieIMU plugin](https://github.com/intel-iot-devkit/FirmataCurieIMU).


```bash
$ mv ConfigurableFirmataCurieImu.ino.bin firmata101.ino.bin
$ mv firmata101.ino.bin /usr/local/share/mraa
```

You will also need to move the imraa config so that imraa can find it.
```bash
$ mv mraa/imraa/imraa.conf /etc/
```

To flash the board run imraa and it will create the lockfile and flash the
Arduino101. You may need root permissions or to add yourself to the uucp or
modem group to access the device node (typically /dev/ttyACM0).
```
$ imraa
```

After this is successful you will be able to use mraa like normal, in the
background mraa will handle talking to the arduino101 by creating
/tmp/imraa.lock which imraa_init() will parse on startup.

