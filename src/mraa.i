%include stdint.i
%include std_string.i

#ifdef DOXYGEN
    %include common_hpp_doc.i
    %include gpio_class_doc.i
    %include i2c_class_doc.i
    %include pwm_class_doc.i
    %include aio_class_doc.i
    %include spi_class_doc.i
#endif

%{
    #include "common.hpp"
    #include "gpio.hpp"
    #include "pwm.hpp"
    #include "i2c.hpp"
    #include "spi.hpp"
    #include "aio.hpp"
%}

%init %{
    //Adding mraa_init() to the module initialisation process
    mraa_init();
%}

%typemap(in) uint8_t = char;
%typemap(in) unsigned char* = char*;
%apply (char *STRING, size_t LENGTH) { (char *data, size_t length) };

%include "common.hpp"

%include "types.h"

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
