%{
    #include "maa.h"
    #include "gpio.hpp"
    #include "pwm.hpp"
    #include "i2c.hpp"
    #include "spi.h"
    #include "aio.hpp"
%}

%init %{
    //Adding maa_init() to the module initialisation process
    maa_init();
%}

%rename(get_version) maa_get_version();
const char * maa_get_version();

#### GPIO ####

%include "gpio.hpp"

#### i2c ####

%include "i2c.hpp"

#### PWM ####

%include "pwm.hpp"

#### SPI ####

%rename(Spi) maa_spi_context;

%ignore spifd;
typedef struct {
    /*@{*/
    int spifd; /**< File descriptor to SPI Device */
    /*@}*/
} maa_spi_context;

%nodefault maa_spi_context;
%extend maa_spi_context {
  maa_spi_context()
  {
    return maa_spi_init();
  }
  ~maa_spi_context()
  {
  }
  int mode(unsigned short mode)
  {
    return maa_spi_mode($self, mode);
  }
  int frequency(int hz)
  {
    return maa_spi_frequency($self, hz);
  }
  unsigned int write(unsigned int data)
  {
    return maa_spi_write($self, data);
  }
}

#### AIO ####

%include "aio.hpp"
