%{
    #include "maa.h"
    #include "gpio.h"
    #include "pwm.h"
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

typedef enum {
    MAA_GPIO_STRONG     = 0, /**< Default. Strong high and low */
    MAA_GPIO_PULLUP     = 1, /**< Resistive High */
    MAA_GPIO_PULLDOWN   = 2, /**< Resistive Low */
    MAA_GPIO_HIZ        = 3  /**< High Z State */
} gpio_mode_t;

typedef enum {
    MAA_GPIO_OUT    = 0, /**< Output. A Mode can also be set */
    MAA_GPIO_IN     = 1  /**< Input. */
} gpio_dir_t;

%nodefault maa_gpio_context;
%rename(Gpio) maa_gpio_context;
%ignore value_fp;

%feature("autodoc") maa_gpio_context "
Create a Gpio object and export it. Depending on your board the correct GPIO
value will be used. If raw is true then the pin that will be initialised will
be the hardcoded pin value in the kernel. Please see your board IO
documentation to understand exactly what will happen.

Parameters:
        * pin: pin number read from the board, i.e IO3 is 3
        * raw: set to True to use real pin value from the kernel";
typedef struct {
    /*@{*/
    int pin; /**< the pin number, as known to the os. */
    FILE *value_fp; /**< the file pointer to the value of the gpio */
#if defined(SWIGPYTHON)
    PyObject *isr; /**< the interupt service request */
#endif
    pthread_t thread_id; /**< the isr handler thread id */
    int isr_value_fp; /**< the isr file pointer on the value */
    /*@}*/
} maa_gpio_context;

%typemap(check) PyObject *pyfunc {
  if (!PyCallable_Check($1))
    SWIG_exception(SWIG_TypeError,"Expected function.");
}

%extend maa_gpio_context {
  maa_gpio_context(int pin, int raw=0)
  {
    if (raw)
      return maa_gpio_init_raw(pin);
    return maa_gpio_init(pin);
  }
  ~maa_gpio_context()
  {
    maa_gpio_unexport($self);
  }
  %feature("autodoc") write "
  Write a value to a GPIO pin

  Parameters:
        * value: value to write to GPIO";
  int write(int value)
  {
    return maa_gpio_write($self, value);
  }
  %feature("autodoc") dir "
  Set the gpio direction

  Parameters:
        * dir: GPIO direction";
  int dir(gpio_dir_t dir)
  {
    return maa_gpio_dir($self, dir);
  }
  %feature("autodoc") read "
  Read the value of a GPIO

  Returns:
        * value: GPIO value";
  int read()
  {
    return maa_gpio_read($self);
  }
  %feature("autodoc") mode "
  Set the GPIO mode

  Parameters:
        * mode: GPIO mode to set";
  int mode(gpio_mode_t mode)
  {
    return maa_gpio_mode($self, mode);
  }
#if defined(SWIGPYTHON)
  //set python method as the isr function
  int set_isr(PyObject *pyfunc)
  {
    Py_INCREF(pyfunc);
    // do a nasty cast to get away from the warnings
    maa_gpio_isr(self, MAA_GPIO_EDGE_BOTH, (void (*) ()) pyfunc);
    return 0;
  }
#else
  %ignore maa_gpio_isr;
#endif
  int isr_exit()
  {
    maa_gpio_isr_exit(self);
  }
}

#### i2c ####

%include "i2c.hpp"

#### PWM ####

%rename(Pwm) maa_pwm_context;

%ignore duty_fp;
typedef struct {
    /*@{*/
    int pin; /**< the pin number, as known to the os. */
    int chipid; /**< the chip id, which the pwm resides */
    FILE *duty_fp; /**< File pointer to duty file */
    /*@}*/
} maa_pwm_context;

%nodefault maa_pwm_context;
%extend maa_pwm_context {
  maa_pwm_context(int pin)
  {
    return maa_pwm_init(pin);
  }
  maa_pwm_context(int chipid, int pin)
  {
    return maa_pwm_init_raw(chipid, pin);
  }
  ~maa_pwm_context()
  {
    maa_pwm_unexport($self);
  }
  int write(float percentage)
  {
    return maa_pwm_write($self, percentage);
  }
  int read()
  {
    return maa_pwm_read($self);
  }
  int period(float seconds)
  {
    return maa_pwm_period($self, seconds);
  }
  int period_ms(int ms)
  {
    return maa_pwm_period_ms($self, ms);
  }
  int period_us(int us)
  {
    return maa_pwm_period_us($self, us);
  }
  int pulsewidth(float seconds)
  {
    return maa_pwm_pulsewidth($self, seconds);
  }
  int pulsewidth_ms(int ms)
  {
    return maa_pwm_pulsewidth($self, ms);
  }
  int pulsewidth_us(int us)
  {
    return maa_pwm_pulsewidth($self, us);
  }
  int enable(int enable)
  {
    return maa_pwm_enable($self, enable);
  }
}

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
