%{
    #include "maa.h"
    #include "gpio.h"
    #include "pwm.h"
    #include "i2c.h"
    #include "spi.h"
%}

%rename(get_version) maa_get_version();
%include "maa.h"

