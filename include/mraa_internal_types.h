/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2014-2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef PERIPHERALMAN
#include <pio/peripheral_manager_client.h>
#else
#include "iio.h"
#endif

#include "common.h"
#include "mraa.h"
#include "mraa_adv_func.h"

// Bionic does not implement pthread cancellation API
#ifndef __BIONIC__
#define HAVE_PTHREAD_CANCEL
#endif

// Max count for various busses
#define MAX_I2C_BUS_COUNT 12
#define MAX_SPI_BUS_COUNT 12
#define MAX_AIO_COUNT 7
#define MAX_UART_COUNT 6
#define MAX_PWM_COUNT 6
#define MAX_LED_COUNT 12

// general status failures for internal functions
#define MRAA_PLATFORM_NO_INIT -3
#define MRAA_IO_SETUP_FAILURE -2
#define MRAA_NO_SUCH_IO -1

// Json platform keys
#define INDEX_KEY "index"
#define NAME_KEY "name"
#define PIN_COUNT_KEY "pin_count"
#define GPIO_COUNT_KEY "gpio_count"
#define AIO_COUNT_KEY "aio_count"
#define SPI_COUNT_KEY "spi_count"
#define I2C_COUNT_KEY "i2c_count"
#define UART_COUNT_KEY "uart_count"
#define PWMDEFAULT_KEY "pwmDefPeriod"
#define PWMMAX_KEY "pwmMaxPeriod"
#define PWMMIN_KEY "pwmMinPeriod"
#define LABEL_KEY "label"
#define DEFAULT_KEY "default"
#define INVALID_KEY "invalid"
#define SCLPIN_KEY "sclpin"
#define SDAPIN_KEY "sdapin"
#define CHIP_ID_KEY "chipID"
#define RAW_PIN_KEY "rawpin"
#define RXPIN_KEY "rx"
#define TXPIN_KEY "tx"
#define UART_PATH_KEY "path"
#define CLOCK_KEY "clock"
#define MISO_KEY "miso"
#define MOSI_KEY "mosi"
#define CS_KEY "chipselect"
#define SS_KEY "slaveselect"
#define PIN_KEY "pin"
#define IO_KEY "layout"
#define PLATFORM_KEY "platform"
#define BUS_KEY "bus"

// IO keys
#define AIO_KEY "a"
#define GPIO_KEY "g"
#define I2C_KEY "i"
#define IIO_KEY "ii"
#define PWM_KEY "p"
#define SPI_KEY "s"
#define UART_KEY "u"
#define UART_OW_KEY "ow"

#define MRAA_JSONPLAT_ENV_VAR "MRAA_JSON_PLATFORM"

#ifdef FIRMATA
struct _firmata {
    /*@*/
    uint8_t feature; /**< the feature */
    uint8_t index;
    void (* isr)(uint8_t*, int); /**< the feature response request */
    mraa_boolean_t added; /**< boolean to set if responses already set in devs array */
    /*@}*/
};
#endif

struct _gpio_group {
    int is_required;
    int dev_fd;
    int gpiod_handle;
    unsigned int gpio_chip;
    /* We can have multiple lines in a gpio group. */
    unsigned int num_gpio_lines;
    unsigned int *gpio_lines;

    /* R/W stuff.*/
    unsigned char *rw_values;
    /* Reverse mapping to original pin number indexes. */
    unsigned int *gpio_group_to_pins_table;

    unsigned int flags;

    /* Event specific fields. */
    int *event_handles;
};

/**
 * A structure representing a gpio pin.
 */
struct _gpio {
    /*@{*/
    int pin; /**< the pin number, as known to the os. */
    int phy_pin; /**< pin passed to clean init. -1 none and raw*/
    int value_fp; /**< the file pointer to the value of the gpio */
    void (* isr)(void *); /**< the interrupt service request */
    void *isr_args; /**< args return when interrupt service request triggered */
    pthread_t thread_id; /**< the isr handler thread id */
    int isr_value_fp; /**< the isr file pointer on the value */
#ifndef HAVE_PTHREAD_CANCEL
    int isr_control_pipe[2]; /**< a pipe used to interrupt the isr from polling the value fd*/
#endif
    mraa_boolean_t isr_thread_terminating; /**< is the isr thread being terminated? */
    mraa_boolean_t owner; /**< If this context originally exported the pin */
    mraa_result_t (*mmap_write) (mraa_gpio_context dev, int value);
    int (*mmap_read) (mraa_gpio_context dev);
    mraa_adv_func_t* advance_func; /**< override function table */
#if defined(MOCKPLAT)
    mraa_gpio_dir_t mock_dir; /**< mock direction of the pin */
    int mock_state; /**< mock state of the pin */
#endif
    /*@}*/
#ifdef PERIPHERALMAN
    AGpio *bgpio;
#endif

    struct _gpio_group *gpio_group;
    unsigned int num_chips;
    int *pin_to_gpio_table;
    unsigned int num_pins;
    mraa_gpio_events_t events;
    int *provided_pins;

    struct _gpio *next;
};

/* Macro for looping over gpio chips. */
#define for_each_gpio_group(group, dev) \
    for (int k = 0; \
        k < dev->num_chips && (group = &dev->gpio_group[k]); \
        ++k) \
            if (dev->gpio_group[k].is_required)

#define for_each_gpio_chip(cinfo, cinfos, num_chips) \
    for (int idx = 0; \
        idx < num_chips && (cinfo = cinfos[idx]); \
        (idx++))

/**
 * A structure representing a I2C bus
 */
struct _i2c {
    /*@{*/
    int busnum; /**< the bus number of the /dev/i2c-* device */
    int fh; /**< the file handle to the /dev/i2c-* device */
    int addr; /**< the address of the i2c slave */
    unsigned long funcs; /**< /dev/i2c-* device capabilities as per https://www.kernel.org/doc/Documentation/i2c/functionality */
    void *handle; /**< generic handle for non-standard drivers that don't use file descriptors  */
    mraa_adv_func_t* advance_func; /**< override function table */
#if defined(MOCKPLAT)
    uint8_t mock_dev_addr; /**< address of the mock I2C device */
    uint8_t mock_dev_data_len; /**< mock device data register block length in bytes */
    uint8_t* mock_dev_data; /**< mock device data register block contents */
#endif
    /*@}*/
#ifdef PERIPHERALMAN
    AI2cDevice *bi2c;
    char bus_name[256];
#endif
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
#ifdef PERIPHERALMAN
    ASpiDevice *bspi;
#endif
};

/**
 * A structure representing a PWM pin
 */
struct _pwm {
    /*@{*/
    int pin; /**< the pin number, as known to the os. */
    int chipid; /**< the chip id, which the pwm resides */
    int duty_fp; /**< File pointer to duty file */
    int period;  /**< Cache the period to speed up setting duty */
    mraa_boolean_t owner; /**< Owner of pwm context*/
    mraa_adv_func_t* advance_func; /**< override function table */
    /*@}*/
#ifdef PERIPHERALMAN
    APwm *bpwm;
#endif
};

/**
 * A structure representing a Analog Input Channel
 */
struct _aio {
    /*@{*/
    unsigned int channel; /**< the channel as on board and ADC module */
    int adc_in_fp; /**< File Pointer to raw sysfs */
    int value_bit; /**< 10 bits by default. Can be increased if board */
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
#if defined(PERIPHERALMAN)
    struct AUartDevice *buart;
#endif
};

#if !defined(PERIPHERALMAN)
/**
 * A structure representing an IIO device
 */
struct _iio {
    int num; /**< IIO device number */
    char* name; /**< IIO device name */
    int fp; /**< IIO device in /dev */
    int fp_event;  /**<  event file descriptor for IIO device */
    void (* isr)(char* data, void* args); /**< the interrupt service request */
    void *isr_args; /**< args return when interrupt service request triggered */
    void (* isr_event)(struct iio_event_data* data, void* args); /**< the event interrupt service request */
    int chan_num;
    pthread_t thread_id; /**< the isr handler thread id */
    mraa_iio_channel* channels;
    int event_num;
    mraa_iio_event* events;
    int datasize;
};
#endif

/**
 * A structure representing an LED device
 */
struct _led {
    /*@{*/
    int count; /**< total LED count in a platform */
    char *led_name; /**< LED name */
    char led_path[64]; /**< sysfs path of the LED */
    int trig_fd; /**< trigger file descriptor */
    int bright_fd; /**< brightness file descriptor */
    int max_bright_fd; /**< maximum brightness file descriptor */
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
 *  Pin commands definition for mraa_mux_t struc
 */

typedef enum {
    PINCMD_UNDEFINED = 0,       // do not modify, default command for zero value, used for backward compatibility with boards where pincmd is not defined (it will be deleted later)
    PINCMD_SET_VALUE = 1,       // set a pin's value
    PINCMD_SET_DIRECTION = 2,   // set a pin's direction (value like MRAA_GPIO_OUT, MRAA_GPIO_OUT_HIGH...)
    PINCMD_SET_IN_VALUE = 3,    // set input direction and value
    PINCMD_SET_OUT_VALUE = 4,   // set output direction and value
    PINCMD_SET_MODE = 5,        // set pin's mode
    PINCMD_SKIP = 6             // just skip this command, do not apply pin and value
} pincmd_t;


/**
 * A Structure representing a multiplexer and the required value
 */
typedef struct {
    /*@{*/
    unsigned int pincmd; /**< Pin command pincmd_xxxx */
                         /**< At this time not all boards will support it -> TO DO */
    unsigned int pin;    /**< Raw GPIO pin id */
    unsigned int value;  /**< Raw GPIO value */
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
    int pinmap; /**< sysfs pin */
    unsigned int parent_id; /** parent chip id */
    unsigned int mux_total; /** Numfer of muxes needed for operation of pin */
    mraa_mux_t mux[6]; /** Array holding information about mux */
    unsigned int output_enable; /** Output Enable GPIO, for level shifting */
    mraa_pin_cap_complex_t complex_cap;

    /* GPIOD_INTERFACE */
    unsigned int gpio_chip;
    unsigned int gpio_line;
    /*@}*/
} mraa_pin_t;

typedef struct {
    /*@{*/
    char mem_dev[32]; /**< Memory device to use /dev/uio0 etc */
    unsigned int mem_sz; /** Size of memory to map */
    unsigned int bit_pos; /** Position of value bit */
    mraa_pin_t gpio; /** GPio context containing none mmap info */
    /*@}*/
} mraa_mmap_pin_t;

/**
 * A Structure representing a physical Pin.
 */
typedef struct {
    /*@{*/
#if defined(PERIPHERALMAN)
    char *name; /**< Peripheral manager's pin name */
#else
    char name[MRAA_PIN_NAME_SIZE]; /**< Pin's real world name */
#endif
    mraa_pincapabilities_t capabilities; /**< Pin Capabiliites */
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
    char *name; /**< i2c bus name */
    int bus_id; /**< ID as exposed in the system */
    int scl; /**< i2c SCL */
    int sda; /**< i2c SDA */
    // mraa_drv_api_t drv_type; /**< Driver type */
    /*@}*/
} mraa_i2c_bus_t;

/**
 * A Structure representing the physical properties of a spi bus.
 */
typedef struct {
    /*@{*/
    char *name; /**< spi bus name */
    unsigned int bus_id; /**< The Bus ID as exposed to the system. */
    unsigned int slave_s; /**< Slave select */
    mraa_boolean_t three_wire; /**< Is the bus only a three wire system */
    int sclk; /**< Serial Clock */
    int mosi; /**< Master Out, Slave In. */
    int miso; /**< Master In, Slave Out. */
    int cs; /**< Chip Select, used when the board is a spi slave */
    /*@}*/
} mraa_spi_bus_t;

/**
 * A Structure representing a uart device.
 */
typedef struct {
    /*@{*/
    char *name; /**< uart name */
    unsigned int index; /**< ID as exposed in the system */
    int rx; /**< uart rx */
    int tx; /**< uart tx */
    int cts; /**< uart cts */
    int rts; /**< uart rts */
    char* device_path; /**< To store "/dev/ttyS1" for example */
    /*@}*/
} mraa_uart_dev_t;

/**
 * A Structure representing a pwm device.
 */
typedef struct {
    /*@{*/
    char *name; /**< pwm device name */
    unsigned int index; /**< ID as exposed in the system */
    char* device_path; /**< To store "/dev/pwm" for example */
    /*@}*/
} mraa_pwm_dev_t;

/**
* A structure representing an aio device.
*/
typedef struct {
    /*@{*/
    unsigned int pin; /**< Pin as exposed in the system */
    /*@}*/
} mraa_aio_dev_t;

/**
 * Structure representing an LED device.
 */
typedef struct {
    /*@{*/
    char *name; /**< LED device function name */
    unsigned int index; /**< Index as exposed in the platform */
    /*@}*/
} mraa_led_dev_t;

/**
 * A Structure representing a platform/board.
 */
typedef struct _board_t {
    /*@{*/
    int phy_pin_count; /**< The Total IO pins on board */
    int gpio_count; /**< GPIO Count */
    int aio_count;  /**< Analog side Count */
    int i2c_bus_count; /**< Usable i2c Count */
    unsigned int aio_non_seq; /**< Are AIO pins non sequential? Usually 0. */
    mraa_i2c_bus_t  i2c_bus[MAX_I2C_BUS_COUNT]; /**< Array of i2c */
    unsigned int def_i2c_bus; /**< Position in array of default i2c bus */
    int spi_bus_count; /**< Usable spi Count */
    mraa_spi_bus_t spi_bus[MAX_SPI_BUS_COUNT];       /**< Array of spi */
    unsigned int def_spi_bus; /**< Position in array of defult spi bus */
    unsigned int adc_raw; /**< ADC raw bit value */
    unsigned int adc_supported; /**< ADC supported bit value */
    unsigned int def_uart_dev; /**< Position in array of default uart */
    unsigned int def_aio_dev; /**< Position in array of default aio */
    unsigned int def_pwm_dev; /**< Position in array of default pwm */
    int uart_dev_count; /**< Usable uart Count */
    mraa_uart_dev_t uart_dev[MAX_UART_COUNT]; /**< Array of UARTs */
    mraa_aio_dev_t aio_dev[MAX_AIO_COUNT]; /**<Array of AIOs */
    mraa_boolean_t no_bus_mux; /**< i2c/spi/adc/pwm/uart bus muxing setup not required */
    int pwm_dev_count; /**< Usable pwm Count */
    mraa_pwm_dev_t pwm_dev[MAX_PWM_COUNT]; /**< Array of PWMs */
    int pwm_default_period; /**< The default PWM period is US */
    int pwm_max_period; /**< Maximum period in us */
    int pwm_min_period; /**< Minimum period in us */
    mraa_platform_t platform_type; /**< Platform type */
    char* platform_name; /**< Platform Name pointer */
    const char* platform_version; /**< Platform versioning info */
    mraa_pininfo_t* pins;     /**< Pointer to pin array */
    mraa_adv_func_t* adv_func;    /**< Pointer to advanced function disptach table */
    struct _board_t* sub_platform;     /**< Pointer to sub platform */
    mraa_boolean_t chardev_capable;  /**< Decide what interface is being used: old sysfs or new char device*/
    mraa_led_dev_t led_dev[MAX_LED_COUNT]; /**< Array of LED devices */
    unsigned int led_dev_count; /**< Total onboard LED device count */
    /*@}*/
} mraa_board_t;

#if !defined(PERIPHERALMAN)
typedef struct {
    struct _iio* iio_devices; /**< Pointer to IIO devices */
    uint8_t iio_device_count; /**< IIO device count */
} mraa_iio_info_t;
#endif

/**
 * Function pointer typedef for use with platform extender libraries.
 * Currently only the FT42222.
 *
 * @param board Pointer to valid board structure.  If a mraa_board_t
 * is initialized, it will be added to board->sub_platform
 *
 * @return MRAA_SUCCESS if a valid subplaform has been initialized,
 * otherwise return MRAA_ERROR_PLATFORM_NOT_INITIALISED
 */
typedef mraa_result_t (*fptr_add_platform_extender)(mraa_board_t* board);
