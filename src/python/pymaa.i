%module(docstring="Python interface to libmaa") pymaa

%feature("autodoc", "3");

#ifdef DOXYGEN
%include ../maa_doc.i
%include ../gpio_doc.i
%include ../i2c_doc.i
%include ../pwm_doc.i
#endif

%include ../maa.i

#### GPIO ####

%rename(Gpio) maa_gpio_context;

typedef struct {
    /*@{*/
    int pin; /**< the pin number, as known to the os. */
    FILE *value_fp; /**< the file pointer to the value of the gpio */
    /*@}*/
} maa_gpio_context;

%nodefault maa_gpio_context;
%extend maa_gpio_context {
  maa_gpio_context(int pin)
  {
    return maa_gpio_init(pin);
  }
  ~maa_gpio_context()
  {
    maa_gpio_close($self);
  }
  int write(int value)
  {
    return maa_gpio_write($self, value);
  }
  int dir(gpio_dir_t dir)
  {
    return maa_gpio_dir($self, dir);
  }
  int read()
  {
    return maa_gpio_read($self);
  }
  int mode(gpio_mode_t mode)
  {
    return maa_gpio_mode($self, mode);
  }
}

#### i2c ####

%rename(I2c) maa_i2c_context;

typedef struct {
    /*@{*/
    int hz; /**< frequency of communication */
    int fh; /**< the file handle to the /dev/i2c-* device */
    int addr; /**< the address of the i2c slave */
    maa_gpio_context gpio;
    /*@}*/
} maa_i2c_context;

%nodefault maa_i2c_context;
%extend maa_i2c_context {
  maa_i2c_context()
  {
    return maa_i2c_init();
  }
  ~maa_i2c_context()
  {
    maa_i2c_stop($self);
  }
  int frequency(int hz)
  {
    return maa_i2c_frequency($self, hz);
  }
  int read(char *data, int length)
  {
    return maa_i2c_read($self, data, length);
  }
  int read()
  {
    return maa_i2c_read_byte($self);
  }
  int write(char *data, int length)
  {
    return maa_i2c_write($self, data, length);
  }
  int write(int data)
  {
    return maa_i2c_write_byte($self, data);
  }
}
