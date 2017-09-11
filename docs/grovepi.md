GrovePi Shield    {#grovepi}
=============================

Mraa can use a GrovePi shield as a subplatform. This means one can utilize the
shield IO as well as the IO on the carrier board. The GrovePi shield is meant
for RPi boards and compatibles such as UP and UP2. It provides convenient GrovePi
style connectors that can be used with various Grove sensors.

Communication with the shield is achieved over I2C using a custom firmware. It
provides access to extra GPIO, AIO and PWM pins.

### Using the subplatform API ###

Using the subplatform API is relatively simple, simply add '512', the platform
offset, to any IO calls.

Example:
 * D3 becomes GPIO 512 + 3 = 515
 * A2 becomes pin 512 + 2 = 514

Keep in mind that the I2C ports on the GrovePi shield are merely a level shifted
extension of the carrier board's I2C bus, hence I2C sensors do not require an
offset.

The API works from UPM or mraa in any of the supported languages and is compiled
with mraa by default. Multiple subplatforms are not yet supported.

### Pinout ###

For a complete diagram of the pins available on these shields please refer to
the hardware section of the [GrovePi Github repository](https://github.com/DexterInd/GrovePi).

### Simple code example ###

This piece of code fades an LED connected to D3 based on the position of a
rotary potentiometer on A2:

```c
#include <stdbool.h>
#include "mraa.h"

void main (int argc, char** argv)
{
    mraa_add_subplatform(MRAA_GROVEPI, "0");

    mraa_aio_context aio = mraa_aio_init(514);
    mraa_pwm_context pwm = mraa_pwm_init(515);

    while(true) {
        mraa_pwm_write(pwm, mraa_aio_read(aio)/1023.0);
        usleep(50000);
    }
}
```

Note that the I2C bus number needs to be passed in as a string to
`mraa_add_subplatform`.

### Limitations & features to be added ###

Currently there is no support for ISRs or the option to query the shield's
firmware version, but these features may be added at a later point.

Theoretically GrovePi Zero shields are compatible too, however they were not
tested. Obviously, only a subset of the IO pins can be used due to the reduced
footprint of this shield.
