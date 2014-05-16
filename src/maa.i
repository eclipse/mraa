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

%rename(get_version) maa_get_version();
const char * maa_get_version();

#### GPIO ####

/**
 * GPIO Output modes
 */
typedef enum {
    MAA_GPIO_STRONG     = 0, /**< Default. Strong high and low */
    MAA_GPIO_PULLUP     = 1, /**< Resistive High */
    MAA_GPIO_PULLDOWN   = 2, /**< Resistive Low */
    MAA_GPIO_HIZ        = 3  /**< High Z State */
} gpio_mode_t;

/**
 * GPIO Direction options.
 */
typedef enum {
    MAA_GPIO_OUT    = 0, /**< Output. A Mode can also be set */
    MAA_GPIO_IN     = 1  /**< Input. */
} gpio_dir_t;

typedef enum {
    MAA_GPIO_EDGE_NONE    = 0, /**< No interrupt on GPIO */
    MAA_GPIO_EDGE_BOTH    = 1, /**< Interupt on rising & falling */
    MAA_GPIO_EDGE_RISING  = 2, /**< Interupt on rising only */
    MAA_GPIO_EDGE_FALLING = 3  /**< Interupt on falling only */
} gpio_edge_t;

%include "gpio.hpp"

#### i2c ####

%include "i2c.hpp"

#### PWM ####

%include "pwm.hpp"

#### SPI ####

%include "spi.hpp"

#### AIO ####

%include "aio.hpp"
