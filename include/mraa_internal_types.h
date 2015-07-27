/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include "common.h"
#include "mraa.h"
#include "mraa_func.h"
#include "mraa_adv_func.h"

// general status failures for internal functions
#define MRAA_PLATFORM_NO_INIT -3
#define MRAA_IO_SETUP_FAILURE -2
#define MRAA_NO_SUCH_IO -1

/**
 * A structure representing a gpio pin.
 */
struct _gpio {
    /*@{*/
    uint16_t pin; /**< the pin number, as known to the os. */
    uint8_t phy_pin; /**< pin passed to clean init. -1 none and raw*/
    int value_fp; /**< the file pointer to the value of the gpio */
    void (* isr)(void *); /**< the interupt service request */
    void *isr_args; /**< args return when interupt service request triggered */
    pthread_t thread_id; /**< the isr handler thread id */
    int isr_value_fp; /**< the isr file pointer on the value */
    mraa_boolean_t isr_thread_terminating; /**< is the isr thread being terminated? */
    mraa_boolean_t owner; /**< If this context originally exported the pin */
    mraa_result_t (*mmap_write) (mraa_gpio_context dev, int value);
    int (*mmap_read) (mraa_gpio_context dev);
    mraa_adv_func_t* advance_func; /**< override function table */
    /*@}*/
};

/**
 * A structure representing a I2C bus
 */
struct _i2c {
    /*@{*/
    uint8_t busnum; /**< the bus number of the /dev/i2c-* device */
    int fh; /**< the file handle to the /dev/i2c-* device */
    uint8_t addr; /**< the address of the i2c slave */
    unsigned long funcs; /**< /dev/i2c-* device capabilities as per https://www.kernel.org/doc/Documentation/i2c/functionality */
    void *handle; /**< generic handle for non-standard drivers that don't use file descriptors  */
    mraa_adv_func_t* advance_func; /**< override function table */
    /*@}*/
};

/**
 * A structure representing the SPI device
 */
struct _spi {
    /*@{*/
    int devfd;          /**< File descriptor to SPI Device */
    uint32_t mode;      /**< Spi mode see spidev.h */
    int clock;          /**< clock to run transactions at */
    mraa_boolean_t lsb; /**< least significant bit mode */
    unsigned int bpw;   /**< Bits per word */
    mraa_adv_func_t* advance_func; /**< override function table */
    /*@}*/
};

/**
 * A structure representing a PWM pin
 */
struct _pwm {
    /*@{*/
    uint16_t pin; /**< the pin number, as known to the os. */
    uint8_t chipid; /**< the chip id, which the pwm resides */
    int duty_fp; /**< File pointer to duty file */
    int period;  /**< Cache the period to speed up setting duty */
    mraa_boolean_t owner; /**< Owner of pwm context*/
    mraa_adv_func_t* advance_func; /**< override function table */
    /*@}*/
};

/**
 * A structure representing a Analog Input Channel
 */
struct _aio {
    /*@{*/
    uint8_t channel; /**< the channel as on board and ADC module */
    int adc_in_fp; /**< File Pointer to raw sysfs */
    uint8_t value_bit; /**< 10 bits by default. Can be increased if board */
    mraa_adv_func_t* advance_func; /**< override function table */
    /*@}*/
};

/**
 * A structure representing a UART device
 */
struct _uart {
    /*@{*/
    int index; /**< the uart index, as known to the os. */
    const char* path; /**< the uart device path. */
    int fd; /**< file descriptor for device. */
    mraa_adv_func_t* advance_func; /**< override function table */
    /*@}*/
};

/**
 * A bitfield representing the capabilities of a pin.
 */
typedef struct {
    /*@{*/
    mraa_boolean_t valid:1;     /**< Is the pin valid at all */
    mraa_boolean_t gpio:1;      /**< Is the pin gpio capable */
    mraa_boolean_t pwm:1;       /**< Is the pin pwm capable */
    mraa_boolean_t fast_gpio:1; /**< Is the pin fast gpio capable */
    mraa_boolean_t spi:1;       /**< Is the pin spi capable */
    mraa_boolean_t i2c:1;       /**< Is the pin i2c capable */
    mraa_boolean_t aio:1;       /**< Is the pin analog input capable */
    mraa_boolean_t uart:1;       /**< Is the pin uart capable */
    /*@}*/
} mraa_pincapabilities_t;

/**
 * A Structure representing a multiplexer and the required value
 */
typedef struct {
    /*@{*/
    uint16_t pin;   /**< Raw GPIO pin id, on linux 3.18+ MAX_GPIOS is 512 */
    mraa_boolean_t value; /**< Raw GPIO value */
    /*@}*/
} mraa_mux_t;

typedef struct {
    mraa_boolean_t complex_pin:1;
    mraa_boolean_t output_en:1;
    mraa_boolean_t output_en_high:1;
    mraa_boolean_t pullup_en:1;
    mraa_boolean_t pullup_en_hiz:1;
} mraa_pin_cap_complex_t;

typedef struct {
    /*@{*/
    uint16_t pinmap; /**< sysfs pin */
    unsigned int parent_id; /** parent chip id */
    uint8_t mux_total; /** Numfer of muxes needed for operation of pin */
    mraa_mux_t mux[4]; /** Array holding information about mux */
    uint16_t output_enable; /** Output Enable GPIO, for level shifting */
    uint16_t pullup_enable; /** Pull-Up enable GPIO, inputs */
    mraa_pin_cap_complex_t complex_cap;
    /*@}*/
} mraa_pin_t;

typedef struct {
    /*@{*/
    uint8_t bit_pos; /** Position of value bit */
    mraa_pin_t gpio; /** GPio context containing none mmap info */
    /*@}*/
} mraa_mmap_pin_t;

/**
 * A Structure representing a physical Pin.
 */
typedef struct {
    /*@{*/
#ifdef BIGPLAT
    char name[MRAA_PIN_NAME_SIZE]; /**< Pin's real world name */
#endif
    mraa_pincapabilities_t capabilites; /**< Pin Capabiliites */
    mraa_pin_t gpio; /**< GPIO structure */
    mraa_pin_t pwm;  /**< PWM structure */
    mraa_pin_t aio;  /**< Anaglog Pin */
    mraa_mmap_pin_t mmap; /**< GPIO through memory */
    mraa_pin_t i2c;  /**< i2c bus/pin */
    mraa_pin_t spi;  /**< spi bus/pin */
    mraa_pin_t uart;  /**< uart module/pin */
    /*@}*/
} mraa_pininfo_t;

/**
 * A Structure representing the physical properties of a i2c bus.
 */
typedef struct {
    /*@{*/
    uint8_t bus_id; /**< ID as exposed in the system */
    uint8_t scl; /**< i2c SCL */
    uint8_t sda; /**< i2c SDA */
    /*@}*/
} mraa_i2c_bus_t;

/**
 * A Structure representing the physical properties of a spi bus.
 */
typedef struct {
    /*@{*/
    uint8_t bus_id; /**< The Bus ID as exposed to the system. */
    uint8_t slave_s; /**< Slave select */
    mraa_boolean_t three_wire; /**< Is the bus only a three wire system */
    uint8_t sclk; /**< Serial Clock */
    uint8_t mosi; /**< Master Out, Slave In. */
    uint8_t miso; /**< Master In, Slave Out. */
    uint8_t cs; /**< Chip Select, used when the board is a spi slave */
    /*@}*/
} mraa_spi_bus_t;

/**
 * A Structure representing a uart device.
 */
typedef struct {
    /*@{*/
    uint8_t index; /**< ID as exposed in the system */
    uint8_t rx; /**< uart rx */
    uint8_t tx; /**< uart tx */
    const char* device_path; /**< To store "/dev/ttyS1" for example */
    /*@}*/
} mraa_uart_dev_t;

/**
 * A Structure representing a platform/board.
 */

typedef struct _board_t {
    /*@{*/
    uint8_t phy_pin_count; /**< The Total IO pins on board */
    uint8_t gpio_count; /**< GPIO Count */
    mraa_pininfo_t* pins;     /**< Pointer to pin array */

    uint8_t i2c_bus_count; /**< Usable i2c Count */
    uint8_t def_i2c_bus; /**< Position in array of default i2c bus */
    mraa_i2c_bus_t i2c_bus[12]; /**< Array of i2c */

    uint8_t spi_bus_count; /**< Usable spi Count */
    uint8_t def_spi_bus; /**< Position in array of defult spi bus */
    mraa_spi_bus_t spi_bus[12]; /**< Array of spi */

    uint8_t uart_dev_count; /**< Usable spi Count */
    uint8_t def_uart_dev; /**< Position in array of defult uart */
    mraa_uart_dev_t uart_dev[6]; /**< Array of UARTs */
    mraa_platform_t platform_type; /**< Platform type */
    mraa_adv_func_t* adv_func;    /**< Pointer to advanced function disptach table */
    struct _board_t* sub_platform;     /**< Pointer to sub platform */

    uint8_t aio_count;  /**< Analog side Count */
    uint8_t adc_raw; /**< ADC raw bit value */
    uint8_t adc_supported; /**< ADC supported bit value */

    uint32_t pwm_default_period; /**< The default PWM period is US */
    uint32_t pwm_max_period; /**< Maximum period in us */
    uint32_t pwm_min_period; /**< Minimum period in us */

    const char* platform_name; /**< Platform Name pointer */
    /*@}*/
} mraa_board_t;


