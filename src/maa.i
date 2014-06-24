%include carrays.i

#ifdef DOXYGEN
    %include common_doc.i
    %include gpio_class_doc.i
    %include i2c_class_doc.i
    %include pwm_class_doc.i
    %include aio_class_doc.i
    %include spi_class_doc.i
#endif

%{
    #include "common.h"
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

%rename(getVersion) maa_get_version;

%rename(setPriority) maa_set_priority;

%rename(printError) maa_result_print(maa_result_t error);

%rename(getPlatform) maa_get_platform_type;

%typemap(in) uint8_t = char;

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
