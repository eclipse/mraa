%{
    #include "maa.h"
    #include "gpio.hpp"
    #include "pwm.hpp"
    #include "i2c.hpp"
    #include "spi.hpp"
    #include "aio.hpp"
%}

%init %{
    //Adding maa_init() to the module initialisation process
    maa_init();
%}

%rename(getVersion) maa_get_version();
const char * maa_get_version();

#### GPIO ####

#### GPIO ####

%include "gpio.hpp"

#### i2c ####

%include "i2c.hpp"

#### PWM ####

%include "pwm.hpp"

#### SPI ####

%include "spi.hpp"

#### AIO ####

%include "aio.hpp"
