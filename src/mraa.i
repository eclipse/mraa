%rename("%(strip:[MRAA_])s", %$isenumitem) "";

%include stdint.i
%include std_string.i
%include exception.i

%{
    #include "common.hpp"
    #include "types.hpp"
    #include "gpio.hpp"
    #include "pwm.hpp"
    #include "i2c.hpp"
    #include "spi.hpp"
    #include "aio.hpp"
    #include "uart.hpp"
    #include "led.hpp"
%}

%exception {
    try {
        $action
    } catch(const std::invalid_argument& e) {
        SWIG_exception(SWIG_ValueError, e.what());
    } catch(...) {
        SWIG_exception(SWIG_RuntimeError, "Unknown exception");
    }
}

%include "types.hpp"

%include "common.hpp"
%template (gpioFromDesc) mraa::initIo<mraa::Gpio>;
%template (aioFromDesc) mraa::initIo<mraa::Aio>;
%template (uartFromDesc) mraa::initIo<mraa::Uart>;
%template (spiFromDesc) mraa::initIo<mraa::Spi>;
%template (i2cFromDesc) mraa::initIo<mraa::I2c>;
%template (pwmFromDesc) mraa::initIo<mraa::Pwm>;
%template (ledFromDesc) mraa::initIo<mraa::Led>;

%ignore Aio(void* aio_context);
%ignore Pwm(void* pwm_context);
%ignore Uart(void* uart_context);
%ignore Spi(void* spi_context);
%ignore I2c(void* i2c_context);
%ignore Gpio(void* gpio_context);
%ignore Led(void* led_context);

%ignore Gpio::nop(uv_work_t* req);
%ignore Gpio::v8isr(uv_work_t* req);
%ignore Gpio::v8isr(uv_work_t* req, int status);
%ignore Gpio::uvwork(void *ctx);
%ignore isr(Edge mode, void (*fptr)(void*), void* args);

%include "gpio.hpp"

%include "i2c.hpp"

%include "pwm.hpp"

%include "spi.hpp"

%include "aio.hpp"

%include "uart.hpp"

%include "led.hpp"
