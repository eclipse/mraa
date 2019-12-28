/*
 * Author: Henry Bruce <henry.bruce@intel.com>
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */


#include "ftdi_ft4222.hpp"
#include "ftd2xx.h"
#include "libft4222.h"
#include "linux/i2c-dev.h"
#include "types.h"
#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cstring>
#include <ctime>
#include <map>
#include <pthread.h>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <vector>

namespace ft4222
{
#define gpioPinsPerFt4222 4

/* These devices have configurable addresses which leave it up to makers of
 * development expander boards to either set these addresses or provide a means
 * to set them (jumpers or solder).  The dev boards for both the PCA9555 and
 * PCA9672 have a slave address of 010 0xxx (where xxx is configurable).
 */
#define PCA9672_ADDR 0x20
#define PCA9555_ADDR 0x27
#define PCA9555_INPUT_REG 0
#define PCA9555_OUTPUT_REG 2
#define PCA9555_POLARITY_REG 4
#define PCA9555_DIRECTION_REG 6
#define PCA9545_ADDR 0x70
#define PCA9672_PINS 8
#define PCA9555_PINS 16
#define PCA9545_BUSSES 4
#define GPIO_PORT_IO_RESET GPIO_PORT2
#define GPIO_PORT_IO_INT GPIO_PORT3
#define MAX_IO_EXPANDER_PINS PCA9555_PINS

/* GPIO expander types */
typedef enum { IO_EXP_NONE, IO_EXP_PCA9672, IO_EXP_PCA9555 } ft4222_io_exp_type;

/* GPIO types */
typedef enum {
    GPIO_TYPE_BUILTIN,
    GPIO_TYPE_PCA9672,
    GPIO_TYPE_PCA9555,
    GPIO_TYPE_UNKNOWN = 99
} ft4222_gpio_type;

/* GPIO expander interrupt monitor */
struct gpio_expander_intmon {
    pthread_t thread;
    pthread_mutex_t mutex;
    mraa_boolean_t should_stop;
    mraa_boolean_t is_interrupt_detected[MAX_IO_EXPANDER_PINS];
    int num_active_pins;
};

/* At present, no c++11 in mraa, so create a lock guard */
class lock_guard
{
  public:
    lock_guard(pthread_mutex_t& mtx) : _mtx(mtx)
    {
        if (pthread_mutex_lock(&_mtx))
            syslog(LOG_ERR, "FT4222 error creating lock on mutex");
    }
    virtual ~lock_guard()
    {
        if (pthread_mutex_unlock(&_mtx))
            syslog(LOG_ERR, "FT4222 error creating lock on mutex");
    }

  private:
    pthread_mutex_t& _mtx;
};

/* Each FT4222 device is represented by 1 Ftdi_4222_Shim instance */
class Ftdi_4222_Shim
{
  public:
    Ftdi_4222_Shim();

    virtual ~Ftdi_4222_Shim();

    static mraa_board_t* init_and_setup_mraa_board();

    bool init_next_free_ftdi4222_device();
    bool setup_io();
    bool init_ftdi_gpios();
    int ft4222_detect_i2c_switch();

    uint32_t ftdi_device_id;
    uint8_t pca9672DirectionMask;
    union {
        uint16_t word;
        uint8_t bytes[2];
    } pca9555OutputValue;
    union {
        uint16_t word;
        uint8_t bytes[2];
    } pca9555DirectionValue;

    gpio_expander_intmon gpio_mon;
    std::vector<GPIO_Dir> pinDirection;

    FT_HANDLE h_gpio;
    FT_HANDLE h_i2c;
    FT_HANDLE h_spi;
    mraa_i2c_mode_t mraa_i2c_mode;
    uint32_t ft4222_i2c_speed();
    pthread_mutex_t mtx_ft4222;
    int cur_i2c_bus;
    ft4222_io_exp_type exp_type;

  private:
    /**
     * Function detects known I2C I/O expanders and returns the number of GPIO pins
     * on expander
     */
    int _detect_io_expander();

    /* Collection to store the board info for 1 FT4222 device */
    mraa_board_t _board;
    /* Collection to store pin info for 1 FT4222 device */
    std::vector<mraa_pininfo_t> _pins;
    /* Struct to store the function table info for 1 FT4222 device */
    mraa_adv_func_t _adv_func_table;
};

Ftdi_4222_Shim::Ftdi_4222_Shim()
: ftdi_device_id(0), pca9672DirectionMask(0), gpio_mon(), pinDirection(4, GPIO_INPUT), h_gpio(NULL),
  h_i2c(NULL), h_spi(NULL), mraa_i2c_mode(MRAA_I2C_FAST), cur_i2c_bus(0), exp_type(IO_EXP_NONE),
  _board(), _adv_func_table()
{
    pca9555OutputValue.word = 0;
    pca9555DirectionValue.word = 0;

    /* Initialize mutexes */
    pthread_mutexattr_t attr_rec;
    pthread_mutexattr_init(&attr_rec);
    pthread_mutexattr_settype(&attr_rec, PTHREAD_MUTEX_RECURSIVE);
    if (pthread_mutex_init(&mtx_ft4222, &attr_rec) != 0) {
        syslog(LOG_ERR, "Failed to setup FT HW device mutex for FT4222 access");
        throw std::runtime_error("Failed to setup FT HW device mutex for FT4222 access");
    }
}

Ftdi_4222_Shim::~Ftdi_4222_Shim()
{
    if (ftdi_device_id)
        syslog(LOG_NOTICE, "Closing FTDI FT4222 device id: 0x%08x", ftdi_device_id);

    if (h_gpio)
        FT4222_UnInitialize(h_gpio);
    if (h_i2c)
        FT4222_UnInitialize(h_i2c);
    if (h_spi)
        FT4222_UnInitialize(h_spi);

    if (h_gpio)
        FT_Close(h_gpio);
    if (h_i2c)
        FT_Close(h_i2c);
    if (h_spi)
        FT_Close(h_spi);
}

/* Global collections for handling multiple shims */
std::vector<Ftdi_4222_Shim> _ftdi_shims;
/* Provide a map of i2c bus index to shim pointer */
std::map<int, Ftdi_4222_Shim*> _i2c_bus_to_shim;
/* Provide a map of gpio bus index to shim pointer */
std::map<int, Ftdi_4222_Shim*> _gpio_pin_to_shim;

uint32_t
Ftdi_4222_Shim::ft4222_i2c_speed()
{
    static std::map<mraa_i2c_mode_t, uint32_t> mode2kHz;
    mode2kHz[MRAA_I2C_STD] = 100;
    mode2kHz[MRAA_I2C_FAST] = 400;
    mode2kHz[MRAA_I2C_HIGH] = 3400;
    return mode2kHz[mraa_i2c_mode];
}

Ftdi_4222_Shim*
ShimFromI2cBus(int bus)
{
    std::map<int, Ftdi_4222_Shim*>::iterator it = _i2c_bus_to_shim.find(bus);
    if (it == _i2c_bus_to_shim.end()) {
        std::ostringstream oss;
        oss << "{";
        for (std::map<int, Ftdi_4222_Shim*>::const_iterator it = _i2c_bus_to_shim.begin();
             it != _i2c_bus_to_shim.end();) {
            oss << it->first;
            if (++it != _i2c_bus_to_shim.end())
                oss << ", ";
        }
        oss << "}";
        syslog(LOG_ERR, "Ftdi_4222_Shim lookup failed for I2C bus: %d, valid busses are: %s", bus,
               oss.str().c_str());
        return NULL;
    }

    return it->second;
}

Ftdi_4222_Shim*
ShimFromGpioPin(int pin)
{
    std::map<int, Ftdi_4222_Shim*>::iterator it = _gpio_pin_to_shim.find(pin);
    if (it == _gpio_pin_to_shim.end()) {
        std::ostringstream oss;
        oss << "{";
        for (std::map<int, Ftdi_4222_Shim*>::const_iterator it = _gpio_pin_to_shim.begin();
             it != _gpio_pin_to_shim.end();) {
            oss << it->first;
            if (++it != _gpio_pin_to_shim.end())
                oss << ", ";
        }
        oss << "}";
        syslog(LOG_ERR, "Ftdi_4222_Shim lookup failed for pin: %d, valid pins are: %s", pin,
               oss.str().c_str());
        return NULL;
    }

    return it->second;
}

mraa_board_t*
Ftdi_4222_Shim::init_and_setup_mraa_board()
{
    /* The Ftdi_4222_Shim constructor can throw */
    try {
        /* Add a shim object to the collection */
        _ftdi_shims.push_back(Ftdi_4222_Shim());
    } catch (...) {
        syslog(LOG_ERR, "Failed to create an Ftdi_4222_Shim instance...");
        return NULL;
    }

    /* Attempt to initialize an FTDI4222 device and setup I/O for use by mraa */
    if (_ftdi_shims.back().init_next_free_ftdi4222_device() && _ftdi_shims.back().setup_io()) {
        return &_ftdi_shims.back()._board;
    } else {
        _ftdi_shims.pop_back();
        return NULL;
    }
}


void
ft4222_sleep_ms(unsigned long mseconds)
{
    struct timespec sleepTime = {};
    // Number of seconds
    sleepTime.tv_sec = mseconds / 1000;
    // Convert fractional seconds to nanoseconds
    sleepTime.tv_nsec = (mseconds % 1000) * 1000000;
    // Iterate nanosleep in a loop until the total sleep time is the original
    // value of the seconds parameter
    while ((nanosleep(&sleepTime, &sleepTime) != 0) && (errno == EINTR))
        ;
}


bool
Ftdi_4222_Shim::init_next_free_ftdi4222_device()
{
    DWORD numDevs = 0;

    if (FT_CreateDeviceInfoList(&numDevs) != FT_OK) {
        syslog(LOG_ERR, "FT_CreateDeviceInfoList failed");
        return false;
    }
    syslog(LOG_NOTICE, "FT_GetDeviceInfoList returned %d devices", numDevs);

    FT_DEVICE_LIST_INFO_NODE devInfo[numDevs];
    if (FT_GetDeviceInfoList(&devInfo[0], &numDevs) != FT_OK) {
        syslog(LOG_ERR, "FT_GetDeviceInfoList failed");
        return false;
    }

    int first_4222_ndx = -1;
    int ftdi_mode = -1;
    /* Look for FT_DEVICE_4222H_0 devices. Print out all devices found. */
    for (DWORD i = 0; i < numDevs; i++) {
        /* Log info for debugging */
        syslog(LOG_NOTICE, "  FTDI ndx: %02d id: 0x%08x %s description: '%s'", i, devInfo[i].ID,
               (devInfo[i].Flags & 0x2) ? "High-speed USB" : "Full-speed USB", &devInfo[i].Description[0]);

        /* If this FTDI device ID is already assigned to an Ftdi_4222_Shim,
         * then log and skip */
        for (std::vector<Ftdi_4222_Shim>::const_iterator it = _ftdi_shims.begin();
             (it != _ftdi_shims.end()) && ((*it).ftdi_device_id != 0); ++it) {
            if ((*it).ftdi_device_id == devInfo[i].ID) {
                syslog(LOG_NOTICE, "  FTDI ndx: %02d id: 0x%08x already initialized, skipping...",
                       i, devInfo[i].ID);
                continue;
            }
        }

        /* FTDI_4222 mode 3 provides 2 devices */
        if ((first_4222_ndx == -1) && (devInfo[i].Type == FT_DEVICE_4222H_0) &&
            ((i + 1 < numDevs) && (devInfo[i].ID == devInfo[i + 1].ID))) {
            first_4222_ndx = i;
            ftdi_mode = 3;
        }
        /* FTDI_4222 mode 0 provides 1 device */
        else if ((first_4222_ndx == -1) && (devInfo[i].Type == FT_DEVICE_4222H_0)) {
            first_4222_ndx = i;
            ftdi_mode = 0;
        }
    }

    /* Was a usable 4222 found? */
    if (first_4222_ndx == -1) {
        syslog(LOG_ERR, "No FT4222 (mode 0 or 3) devices found.");
        return false;
    }

    syslog(LOG_NOTICE, "FTDI 4222 device found at ndx: %02d, mode %d (%s)", first_4222_ndx, ftdi_mode,
           ftdi_mode == 0 ? "SPI/I2C" : ftdi_mode == 3 ? "SPI/I2C and GPIO" : "Unknown mode");

    /* Both modes provide a SPI/I2C at the first ndx */
    syslog(LOG_NOTICE, "FTDI ndx: %02d initializing as SPI/I2C", first_4222_ndx);

    /* Setup I2c */
    if (devInfo[first_4222_ndx].LocId == 0) {
        syslog(LOG_ERR, "No I2C controller for FTDI_4222 device");
        return false;
    }

    if (FT_OpenEx(reinterpret_cast<PVOID>(devInfo[first_4222_ndx].LocId), FT_OPEN_BY_LOCATION, &h_i2c) != FT_OK) {
        syslog(LOG_NOTICE, "FTDI ndx: %02d initializing as SPI/I2C device - FAILED to open", first_4222_ndx + 1);
        return false;
    }

    // Tell the FT4222 to be an I2C Master by default on init.
    if (FT4222_I2CMaster_Init(h_i2c, ft4222_i2c_speed()) != FT4222_OK) {
        syslog(LOG_NOTICE, "FTDI ndx: %02d initializing as SPI/I2C device - FAILED to "
                           "initialize",
               first_4222_ndx + 1);
        return false;
    }

    if (FT4222_I2CMaster_Reset(h_i2c) != FT4222_OK) {
        syslog(LOG_NOTICE, "FTDI ndx: %02d initializing as SPI/I2C device - FAILED to reset",
               first_4222_ndx + 1);
        return false;
    }

    /* Mode 3 adds 1 GPIO device, setup GPIO */
    if (ftdi_mode == 3) {
        syslog(LOG_NOTICE, "FTDI ndx: %02d initializing as GPIO device", first_4222_ndx + 1);

        DWORD locationIdGpio = devInfo[first_4222_ndx + 1].LocId;
        if (locationIdGpio == 0) {
            syslog(LOG_ERR, "No GPIO controller for FTDI_4222 device");
            return false;
        }

        if (FT_OpenEx((PVOID)(uintptr_t) locationIdGpio, FT_OPEN_BY_LOCATION, &h_gpio) != FT_OK) {
            syslog(LOG_NOTICE, "FTDI ndx: %02d initializing as GPIO device - FAILED to open",
                   first_4222_ndx + 1);
            return false;
        }

        if (!init_ftdi_gpios()) {
            syslog(LOG_NOTICE, "FTDI ndx: %02d initializing as GPIO device - FAILED to initialize",
                   first_4222_ndx + 1);
            return false;
        }
    }

    /* Save off this FTDI device ID */
    ftdi_device_id = devInfo[first_4222_ndx].ID;

    syslog(LOG_NOTICE, "FTDI FT4222 device initialization completed successfully");
    return true;
}

int
ft4222_i2c_read_internal(Ftdi_4222_Shim& shim, uint8_t addr, uint8_t* data, int length)
{
    uint16 bytesRead = 0;
    uint8 controllerStatus = 0;

    /* If a read fails, check the I2C controller status, reset the controller.
     *
     * In some cases a master read will return FT4222_OK but leaves the bus in
     * an error state.
     * */
    FT4222_STATUS sts_rd = FT4222_I2CMaster_Read(shim.h_i2c, addr, data, length, &bytesRead);

    if ((sts_rd != FT4222_OK) || ((FT4222_I2CMaster_GetStatus(shim.h_i2c, &controllerStatus) != FT4222_OK) ||
                                  (controllerStatus & 0x2))) {
        FT4222_I2CMaster_GetStatus(shim.h_i2c, &controllerStatus);

        syslog(LOG_ERR, "FT4222_I2CMaster_Read failed for address 0x%02x with code 0x%02x I2C "
                        "controller status: 0x%02x",
               addr, sts_rd, controllerStatus);
        FT4222_I2CMaster_Reset(shim.h_i2c);
        return 0;
    }

    return bytesRead;
}

int
ft4222_i2c_write_internal(Ftdi_4222_Shim& shim, uint8_t addr, uint8_t* data, int bytesToWrite)
{
    uint16 bytesWritten = 0;
    uint8 controllerStatus;

    /* If a write fails, check the I2C controller status, reset the controller,
     * return 0? */
    if (FT4222_I2CMaster_Write(shim.h_i2c, addr, data, bytesToWrite, &bytesWritten) != FT4222_OK) {
        FT4222_I2CMaster_GetStatus(shim.h_i2c, &controllerStatus);

        syslog(LOG_ERR, "FT4222_I2CMaster_Write failed address %#02x. Code %d", addr, controllerStatus);
        FT4222_I2CMaster_Reset(shim.h_i2c);
        bytesWritten = 0;
    }

    if (bytesWritten != bytesToWrite)
        syslog(LOG_ERR, "FT4222_I2CMaster_Write wrote %u of %u bytes.", bytesWritten, bytesToWrite);

    return bytesWritten;
}
mraa_result_t
ft4222_i2c_select_bus(int bus)
{
    Ftdi_4222_Shim* shim = ShimFromI2cBus(bus);
    if (!shim)
        return MRAA_ERROR_NO_RESOURCES;

    if (bus > 0 && bus != shim->cur_i2c_bus) {
        syslog(LOG_NOTICE, "ft4222_i2c_select_bus switching to bus %d", bus);
        uint8_t data;
        if (bus == 0)
            data = 0;
        else
            data = 1 << (bus - 1);

        if (ft4222_i2c_write_internal(*shim, PCA9545_ADDR, &data, 1) == 1)
            shim->cur_i2c_bus = bus;
        else
            return MRAA_ERROR_UNSPECIFIED;
    }
    return MRAA_SUCCESS;
}

int
ft4222_i2c_context_read(mraa_i2c_context dev, uint8_t* data, int length)
{
    Ftdi_4222_Shim* shim = ShimFromI2cBus(dev->busnum);
    if (!shim)
        return -1;

    int bytes_read = 0;
    if (ft4222_i2c_select_bus(dev->busnum) == MRAA_SUCCESS)
        bytes_read = ft4222_i2c_read_internal(*shim, dev->addr, data, length);
    return bytes_read;
}

int
ft4222_i2c_context_write(mraa_i2c_context dev, uint8_t* data, int length)
{
    Ftdi_4222_Shim* shim = ShimFromI2cBus(dev->busnum);
    if (!shim)
        return -1;

    int bytes_written = 0;
    if (ft4222_i2c_select_bus(dev->busnum) == MRAA_SUCCESS)
        bytes_written = ft4222_i2c_write_internal(*shim, dev->addr, data, length);
    return bytes_written;
}

int
Ftdi_4222_Shim::_detect_io_expander()
{
    uint8_t data = 0;
    if (ft4222_i2c_read_internal(*this, PCA9672_ADDR, &data, 1) == 1) {
        syslog(LOG_ERR, "Detected I/O expander: PCA9672 with %d I/O pins", PCA9672_PINS);
        exp_type = IO_EXP_PCA9672;
        return PCA9672_PINS;
    } else {
        uint8_t reg = PCA9555_INPUT_REG;
        ft4222_i2c_write_internal(*this, PCA9555_ADDR, &reg, 1);
        if (ft4222_i2c_read_internal(*this, PCA9555_ADDR, &data, 1) == 1) {
            syslog(LOG_ERR, "Detected I/O expander: PCA9555 with %d I/O pins", PCA9555_PINS);
            exp_type = IO_EXP_PCA9555;
            reg = PCA9555_OUTPUT_REG;

            ft4222_i2c_write_internal(*this, PCA9555_ADDR, &reg, 1);
            ft4222_i2c_read_internal(*this, PCA9555_ADDR, &pca9555OutputValue.bytes[0], 2);

            reg = PCA9555_DIRECTION_REG;

            ft4222_i2c_write_internal(*this, PCA9555_ADDR, &reg, 1);
            ft4222_i2c_read_internal(*this, PCA9555_ADDR, &pca9555DirectionValue.bytes[0], 2);
            return PCA9555_PINS;
        }
    }
    exp_type = IO_EXP_NONE;
    return 0;
}

ft4222_gpio_type
ft4222_get_gpio_type(int pin)
{
    Ftdi_4222_Shim* shim = ShimFromGpioPin(pin);
    if (!shim)
        return GPIO_TYPE_UNKNOWN;

    if (pin < gpioPinsPerFt4222) {
        return GPIO_TYPE_BUILTIN;
    } else
        switch (shim->exp_type) {
            case IO_EXP_PCA9672:
                return GPIO_TYPE_PCA9672;
            case GPIO_TYPE_PCA9555:
                return GPIO_TYPE_PCA9555;
            default:
                return GPIO_TYPE_UNKNOWN;
        }
}

mraa_result_t
ftdi_ft4222_set_internal_gpio_dir(Ftdi_4222_Shim& shim, int physical_pin, GPIO_Dir direction)
{
    /* Update the direction for this pin */
    shim.pinDirection[physical_pin] = direction;

    if (!shim.init_ftdi_gpios())
        return MRAA_ERROR_UNSPECIFIED;

    return MRAA_SUCCESS;
}

mraa_result_t
ft4222_gpio_set_pca9672_dir(mraa_gpio_context dev, mraa_gpio_dir_t dir)
{
    Ftdi_4222_Shim* shim = ShimFromGpioPin(dev->phy_pin);
    if (!shim)
        return MRAA_ERROR_UNSPECIFIED;

    uint8_t mask = 1 << dev->phy_pin;
    switch (dir) {
        case MRAA_GPIO_IN: {
            shim->pca9672DirectionMask |= mask;
            int bytes_written =
            ft4222_i2c_write_internal(*shim, PCA9672_ADDR, &shim->pca9672DirectionMask, 1);
            return bytes_written == 1 ? MRAA_SUCCESS : MRAA_ERROR_UNSPECIFIED;
        }
        case MRAA_GPIO_OUT: {
            shim->pca9672DirectionMask &= (~mask);
            return MRAA_SUCCESS;
        }
        default:
            return MRAA_ERROR_UNSPECIFIED;
    }
}


mraa_result_t
ft4222_gpio_set_pca9555_dir(mraa_gpio_context dev, mraa_gpio_dir_t dir)
{
    Ftdi_4222_Shim* shim = ShimFromGpioPin(dev->phy_pin);
    if (!shim)
        return MRAA_ERROR_UNSPECIFIED;

    uint16_t mask = 1 << (dev->phy_pin - gpioPinsPerFt4222);
    switch (dir) {
        case MRAA_GPIO_IN:
            shim->pca9555DirectionValue.word |= mask;
            break;
        case MRAA_GPIO_OUT:
            shim->pca9555DirectionValue.word &= (~mask);
            break;
        default: {
            syslog(LOG_ERR, "Invalid gpio direction: 0x%08x", dir);
            return MRAA_ERROR_UNSPECIFIED;
        }
    }
    uint8_t buf[3] = { PCA9555_DIRECTION_REG, shim->pca9555DirectionValue.bytes[0],
                       shim->pca9555DirectionValue.bytes[1] };

    int bytes_written = ft4222_i2c_write_internal(*shim, PCA9555_ADDR, &buf[0], sizeof(buf));
    return bytes_written == sizeof(buf) ? MRAA_SUCCESS : MRAA_ERROR_UNSPECIFIED;
}

mraa_result_t
ftdi_ft4222_set_internal_gpio_trigger(int pin, GPIO_Trigger trigger)
{
    Ftdi_4222_Shim* shim = ShimFromGpioPin(pin);
    if (!shim)
        return MRAA_ERROR_NO_RESOURCES;

    FT4222_STATUS ft4222Status =
    FT4222_GPIO_SetInputTrigger(shim->h_gpio, static_cast<GPIO_Port>(pin), trigger);
    if (ft4222Status == FT4222_OK)
        return MRAA_SUCCESS;
    else {
        syslog(LOG_ERR, "FT4222_GPIO_SetInputTrigger failed with FT4222_STATUS: 0x%04x", ft4222Status);
        return MRAA_ERROR_UNSPECIFIED;
    }
}

// Function detects known I2C switches and returns the number of busses.
// On startup switch is disabled so default bus will be integrated i2c bus.
int
Ftdi_4222_Shim::ft4222_detect_i2c_switch()
{
    uint8_t data = 0;
    if (ft4222_i2c_read_internal(*this, PCA9545_ADDR, &data, 1) == 1) {
        data = 0;
        return ft4222_i2c_write_internal(*this, PCA9545_ADDR, &data, 1) == 1 ? PCA9545_BUSSES : 0;
    }
    return 0;
}

/******************* I2C functions *******************/

mraa_result_t
i2c_init_bus_replace(mraa_i2c_context dev)
{
    Ftdi_4222_Shim* shim = ShimFromI2cBus(dev->busnum);
    if (!shim)
        return MRAA_ERROR_NO_RESOURCES;

    lock_guard lock(shim->mtx_ft4222);

    // Tell the FT4222 to be an I2C Master.
    FT4222_STATUS ft4222Status = FT4222_I2CMaster_Init(shim->h_i2c, shim->ft4222_i2c_speed());
    if (FT4222_OK != ft4222Status) {
        syslog(LOG_ERR, "FT4222_I2CMaster_Init failed (error %d)!", ft4222Status);
        return MRAA_ERROR_NO_RESOURCES;
    }

    // Reset the I2CM registers to a known state.
    ft4222Status = FT4222_I2CMaster_Reset(shim->h_i2c);
    if (FT4222_OK != ft4222Status) {
        syslog(LOG_ERR, "FT4222_I2CMaster_Reset failed (error %d)!", ft4222Status);
        return MRAA_ERROR_NO_RESOURCES;
    }

    syslog(LOG_NOTICE, "I2C interface enabled GPIO0 and GPIO1 will be unavailable.");
    dev->handle = shim->h_i2c;

    // Don't use file descriptors
    dev->fh = -1;

    // Advertise minimal i2c support as per
    // https://www.kernel.org/doc/Documentation/i2c/functionality
    dev->funcs = I2C_FUNC_I2C;
    return MRAA_SUCCESS;
}

mraa_result_t
i2c_set_frequency_replace(mraa_i2c_context dev, mraa_i2c_mode_t mode)
{
    Ftdi_4222_Shim* shim = ShimFromI2cBus(dev->busnum);
    if (!shim)
        return MRAA_ERROR_NO_RESOURCES;

    lock_guard lock(shim->mtx_ft4222);

    /* Save off this speed */
    shim->mraa_i2c_mode = mode;

    return FT4222_I2CMaster_Init(shim->h_i2c, shim->ft4222_i2c_speed()) == FT4222_OK ? MRAA_SUCCESS : MRAA_ERROR_UNSPECIFIED;
}

mraa_result_t
i2c_address_replace(mraa_i2c_context dev, uint8_t addr)
{
    dev->addr = (int) addr;
    return MRAA_SUCCESS;
}

int
i2c_read_replace(mraa_i2c_context dev, uint8_t* data, int length)
{
    Ftdi_4222_Shim* shim = ShimFromI2cBus(dev->busnum);
    if (!shim)
        return -1;

    lock_guard lock(shim->mtx_ft4222);

    int bytes_read = ft4222_i2c_read_internal(*shim, dev->addr, data, length);

    return bytes_read;
}

int
i2c_read_byte_replace(mraa_i2c_context dev)
{
    Ftdi_4222_Shim* shim = ShimFromI2cBus(dev->busnum);
    if (!shim)
        return -1;

    lock_guard lock(shim->mtx_ft4222);

    uint8_t data = 0;
    int bytes_read = ft4222_i2c_context_read(dev, &data, 1);
    return bytes_read == 1 ? data : -1;
}

int
i2c_read_byte_data_replace(mraa_i2c_context dev, uint8_t command)
{
    Ftdi_4222_Shim* shim = ShimFromI2cBus(dev->busnum);
    if (!shim)
        return -1;

    lock_guard lock(shim->mtx_ft4222);

    uint8_t data;
    int bytes_read = 0;

    uint16 bytesWritten = ft4222_i2c_context_write(dev, &command, 1);

    if (bytesWritten == 1)
        bytes_read = ft4222_i2c_context_read(dev, &data, 1);

    if (bytes_read == 1) {
        return (int) data;
    }
    return -1;
}

int
i2c_read_word_data_replace(mraa_i2c_context dev, uint8_t command)
{
    Ftdi_4222_Shim* shim = ShimFromI2cBus(dev->busnum);
    if (!shim)
        return -1;

    lock_guard lock(shim->mtx_ft4222);

    union {
        uint8_t bytes[2];
        uint16_t word;
    } buf = {};
    int bytes_read = 0;

    int bytes_written = ft4222_i2c_context_write(dev, &command, 1);
    if (bytes_written == 1)
        bytes_read = ft4222_i2c_context_read(dev, &buf.bytes[0], 2);

    if (bytes_read == 2)
        return (int) buf.word;

    return -1;
}

int
i2c_read_bytes_data_replace(mraa_i2c_context dev, uint8_t command, uint8_t* data, int length)
{
    Ftdi_4222_Shim* shim = ShimFromI2cBus(dev->busnum);
    if (!shim)
        return -1;

    lock_guard lock(shim->mtx_ft4222);

    int bytes_read = 0;
    int bytes_written = ft4222_i2c_context_write(dev, &command, 1);
    if (bytes_written == 1)
        bytes_read = ft4222_i2c_context_read(dev, data, length);
    return bytes_read;
}

mraa_result_t
i2c_write_replace(mraa_i2c_context dev, const uint8_t* data, int bytesToWrite)
{
    Ftdi_4222_Shim* shim = ShimFromI2cBus(dev->busnum);
    if (!shim)
        return MRAA_ERROR_NO_RESOURCES;

    lock_guard lock(shim->mtx_ft4222);
    uint16 bytesWritten = ft4222_i2c_context_write(dev, (uint8_t*) data, bytesToWrite);
    return bytesToWrite == bytesWritten ? MRAA_SUCCESS : MRAA_ERROR_INVALID_HANDLE;
}

/* No mutex needed */
mraa_result_t
i2c_write_byte_replace(mraa_i2c_context dev, uint8_t data)
{
    mraa_result_t status = i2c_write_replace(dev, &data, 1);
    return status;
}

/* No mutex needed */
mraa_result_t
i2c_write_byte_data_replace(mraa_i2c_context dev, const uint8_t data, const uint8_t command)
{
    uint8_t buf[2] = { command, data };
    mraa_result_t status = i2c_write_replace(dev, &buf[0], 2);
    return status;
}

/* No mutex needed */
mraa_result_t
i2c_write_word_data_replace(mraa_i2c_context dev, const uint16_t data, const uint8_t command)
{
    uint8_t buf[3] = { command, (uint8_t) data, (uint8_t)(data >> 8) };
    mraa_result_t status = i2c_write_replace(dev, &buf[0], 3);
    return status;
}

mraa_result_t i2c_stop_replace(mraa_i2c_context /*dev*/)
{
    return MRAA_SUCCESS;
}

//        /******************* GPIO functions *******************/
//
mraa_result_t
gpio_init_internal_replace(mraa_gpio_context dev, int pin)
{
    Ftdi_4222_Shim* shim = ShimFromGpioPin(dev->phy_pin);
    if (!shim)
        return MRAA_ERROR_NO_RESOURCES;

    lock_guard lock(shim->mtx_ft4222);

    /* Keep the subplatform index as pin.  Example: 516 */
    /* And the phy_pin as the local index to pin.  Example: 516 - 512 = 4 */
    dev->pin = pin;
    dev->phy_pin = pin & (~MRAA_SUB_PLATFORM_MASK);

    if (pin < 2) {
        syslog(LOG_NOTICE, "Closing I2C interface to enable GPIO%d", pin);

        /* Replace with call to SPI init when SPI is fully implemented */
        FT4222_STATUS ft4222Status =
        FT4222_SPIMaster_Init(shim->h_gpio, SPI_IO_SINGLE, CLK_DIV_4, CLK_IDLE_HIGH, CLK_LEADING, 0x01);
        if (FT4222_OK != ft4222Status) {
            syslog(LOG_ERR, "Failed to close I2C interface and start SPI (error %d)!", ft4222Status);
            return MRAA_ERROR_NO_RESOURCES;
        }
    }
    return MRAA_SUCCESS;
}

mraa_result_t gpio_mode_replace(mraa_gpio_context /*dev*/, mraa_gpio_mode_t /*mode*/)
{
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

mraa_result_t
gpio_edge_mode_replace(mraa_gpio_context dev, mraa_gpio_edge_t mode)
{
    Ftdi_4222_Shim* shim = ShimFromGpioPin(dev->phy_pin);
    if (!shim)
        return MRAA_ERROR_NO_RESOURCES;

    lock_guard lock(shim->mtx_ft4222);

    mraa_result_t result = MRAA_SUCCESS;

    switch (ft4222_get_gpio_type(dev->phy_pin)) {
        case GPIO_TYPE_BUILTIN:
            switch (mode) {
                case MRAA_GPIO_EDGE_NONE:
                case MRAA_GPIO_EDGE_BOTH:
                    result = ftdi_ft4222_set_internal_gpio_trigger(
                    dev->phy_pin, static_cast<GPIO_Trigger>(GPIO_TRIGGER_RISING | GPIO_TRIGGER_FALLING));
                    break;
                case MRAA_GPIO_EDGE_RISING:
                    result = ftdi_ft4222_set_internal_gpio_trigger(dev->phy_pin, GPIO_TRIGGER_RISING);
                    break;
                case MRAA_GPIO_EDGE_FALLING:
                    result = ftdi_ft4222_set_internal_gpio_trigger(dev->phy_pin, GPIO_TRIGGER_FALLING);
                    break;
                default:
                    result = MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
                    break;
            }
            break;
        case GPIO_TYPE_PCA9672:
        case GPIO_TYPE_PCA9555:
            break;
        default:
            result = MRAA_ERROR_INVALID_RESOURCE;
            break;
    }

    return result;
}

mraa_result_t
ft4222_i2c_read_io_expander(Ftdi_4222_Shim& shim, uint16_t* value)
{
    int bytes_read = 0;
    uint8_t reg = PCA9555_INPUT_REG;

    switch (shim.exp_type) {
        case IO_EXP_PCA9672:
            bytes_read = ft4222_i2c_read_internal(shim, PCA9672_ADDR, (uint8_t*) value, 1);
            break;
        case GPIO_TYPE_PCA9555:
            if (ft4222_i2c_write_internal(shim, PCA9555_ADDR, &reg, 1) == 1)
                bytes_read = ft4222_i2c_read_internal(shim, PCA9555_ADDR, (uint8_t*) value, 2);
            break;
        default:;
    }
    return bytes_read > 0 ? MRAA_SUCCESS : MRAA_ERROR_UNSPECIFIED;
}

int
gpio_read_replace(mraa_gpio_context dev)
{
    Ftdi_4222_Shim* shim = ShimFromGpioPin(dev->phy_pin);
    if (!shim)
        return -1;

    lock_guard lock(shim->mtx_ft4222);

    switch (ft4222_get_gpio_type(dev->phy_pin)) {
        case GPIO_TYPE_BUILTIN: {
            BOOL value;
            FT4222_STATUS ft4222Status =
            FT4222_GPIO_Read(shim->h_gpio, static_cast<GPIO_Port>(dev->phy_pin), &value);
            if (FT4222_OK != ft4222Status) {
                syslog(LOG_ERR, "FT4222_GPIO_Read failed (error %d)!", ft4222Status);
                return -1;
            }
            return value;
        }
        case GPIO_TYPE_PCA9672:
        case GPIO_TYPE_PCA9555: {
            uint16_t mask = 1 << (dev->phy_pin - gpioPinsPerFt4222);
            uint16_t value;
            mraa_result_t res = ft4222_i2c_read_io_expander(*shim, &value);
            return res == MRAA_SUCCESS ? (value & mask) == mask : -1;
        }
        default:
            return -1;
    }
}

mraa_result_t
gpio_write_replace_wrapper(mraa_gpio_context dev, int write_value, bool require_atomic = true)
{
    Ftdi_4222_Shim* shim = ShimFromGpioPin(dev->phy_pin);
    if (!shim)
        return MRAA_ERROR_NO_RESOURCES;

    if (require_atomic)
        pthread_mutex_lock(&shim->mtx_ft4222);

    mraa_result_t result = MRAA_SUCCESS;

    switch (ft4222_get_gpio_type(dev->phy_pin)) {
        case GPIO_TYPE_BUILTIN: {
            FT4222_STATUS ft4222Status =
            FT4222_GPIO_Write(shim->h_gpio, static_cast<GPIO_Port>(dev->phy_pin), write_value);
            if (FT4222_OK != ft4222Status) {
                syslog(LOG_ERR, "FT4222_GPIO_Write to pin %i failed (error %d)!",
                        dev->phy_pin,
                        ft4222Status);
                result = MRAA_ERROR_UNSPECIFIED;
            }
        } break;
        case GPIO_TYPE_PCA9672: {
            uint8_t mask = 1 << dev->phy_pin;
            uint8_t value;
            int bytes_written = 0;
            int bytes_read = ft4222_i2c_read_internal(*shim, PCA9672_ADDR, &value, 1);
            if (bytes_read == 1) {
                if (write_value == 1)
                    value = value | mask | shim->pca9672DirectionMask;
                else
                    value &= (~mask);
                bytes_written = ft4222_i2c_write_internal(*shim, PCA9672_ADDR, &value, 1);
            }
            result = bytes_written == 1 ? MRAA_SUCCESS : MRAA_ERROR_UNSPECIFIED;
        } break;
        case GPIO_TYPE_PCA9555: {
            uint16_t mask = 1 << (dev->phy_pin - gpioPinsPerFt4222);
            if (write_value)
                shim->pca9555OutputValue.word |= mask;
            else
                shim->pca9555OutputValue.word &= (~mask);
            uint8_t buf[3] = { PCA9555_OUTPUT_REG, shim->pca9555OutputValue.bytes[0],
                               shim->pca9555OutputValue.bytes[1] };
            int bytes_written = ft4222_i2c_write_internal(*shim, PCA9555_ADDR, &buf[0], sizeof(buf));
            result = bytes_written == sizeof(buf) ? MRAA_SUCCESS : MRAA_ERROR_UNSPECIFIED;
        } break;
        default:
            result = MRAA_ERROR_INVALID_RESOURCE;
            break;
    }

    if (require_atomic)
        pthread_mutex_unlock(&shim->mtx_ft4222);

    return result;
}

mraa_result_t
gpio_write_replace(mraa_gpio_context dev, int write_value)
{
    Ftdi_4222_Shim* shim = ShimFromGpioPin(dev->phy_pin);
    if (!shim)
        return MRAA_ERROR_NO_RESOURCES;

    lock_guard lock(shim->mtx_ft4222);

    /* Default to an atomic gpio_write_replace call */
    return gpio_write_replace_wrapper(dev, write_value);
}

mraa_result_t
gpio_dir_replace(mraa_gpio_context dev, mraa_gpio_dir_t dir)
{
    Ftdi_4222_Shim* shim = ShimFromGpioPin(dev->phy_pin);
    if (!shim)
        return MRAA_ERROR_NO_RESOURCES;

    lock_guard lock(shim->mtx_ft4222);

    switch (ft4222_get_gpio_type(dev->phy_pin)) {
        case GPIO_TYPE_BUILTIN:
            switch (dir) {
                case MRAA_GPIO_IN:
                    return ftdi_ft4222_set_internal_gpio_dir(*shim, dev->phy_pin, GPIO_INPUT);
                case MRAA_GPIO_OUT:
                    return ftdi_ft4222_set_internal_gpio_dir(*shim, dev->phy_pin, GPIO_OUTPUT);
                case MRAA_GPIO_OUT_HIGH:
                    if (ftdi_ft4222_set_internal_gpio_dir(*shim, dev->phy_pin, GPIO_OUTPUT) != MRAA_SUCCESS)
                        return MRAA_ERROR_UNSPECIFIED;
                    return gpio_write_replace_wrapper(dev, 1, false);
                case MRAA_GPIO_OUT_LOW:
                    if (ftdi_ft4222_set_internal_gpio_dir(*shim, dev->phy_pin, GPIO_OUTPUT) != MRAA_SUCCESS)
                        return MRAA_ERROR_UNSPECIFIED;
                    return gpio_write_replace_wrapper(dev, 0, false);
                default:
                    return MRAA_ERROR_INVALID_PARAMETER;
            }
        case GPIO_TYPE_PCA9672:
            switch (dir) {
                case MRAA_GPIO_IN:
                case MRAA_GPIO_OUT:
                    return ft4222_gpio_set_pca9672_dir(dev, dir);
                case MRAA_GPIO_OUT_HIGH:
                    if (ft4222_gpio_set_pca9672_dir(dev, dir) != MRAA_SUCCESS)
                        return MRAA_ERROR_UNSPECIFIED;
                    return gpio_write_replace_wrapper(dev, 1, false);
                case MRAA_GPIO_OUT_LOW:
                    if (ft4222_gpio_set_pca9672_dir(dev, dir) != MRAA_SUCCESS)
                        return MRAA_ERROR_UNSPECIFIED;
                    return gpio_write_replace_wrapper(dev, 0, false);
                default:
                    return MRAA_ERROR_INVALID_PARAMETER;
            }
        case GPIO_TYPE_PCA9555:
            switch (dir) {
                case MRAA_GPIO_IN:
                case MRAA_GPIO_OUT:
                    return ft4222_gpio_set_pca9555_dir(dev, dir);
                case MRAA_GPIO_OUT_HIGH:
                    if (ft4222_gpio_set_pca9555_dir(dev, dir) != MRAA_SUCCESS)
                        return MRAA_ERROR_UNSPECIFIED;
                    return gpio_write_replace_wrapper(dev, 1, false);
                case MRAA_GPIO_OUT_LOW:
                    if (ft4222_gpio_set_pca9555_dir(dev, dir) != MRAA_SUCCESS)
                        return MRAA_ERROR_UNSPECIFIED;
                    return gpio_write_replace_wrapper(dev, 0, false);
                default:
                    return MRAA_ERROR_INVALID_PARAMETER;
            }
        default:
            return MRAA_ERROR_INVALID_RESOURCE;
    }
}

mraa_boolean_t
ft4222_has_internal_gpio_triggered(Ftdi_4222_Shim& shim, int physical_pin)
{
    uint16 num_events = 0;
    FT4222_GPIO_GetTriggerStatus(shim.h_gpio, static_cast<GPIO_Port>(physical_pin), &num_events);

    if (num_events > 0) {
        int i;
        uint16 num_events_read;
        GPIO_Trigger event;
        for (i = 0; i < num_events; ++i) {
            FT4222_GPIO_ReadTriggerQueue(shim.h_gpio, static_cast<GPIO_Port>(physical_pin), &event,
                                         1, &num_events_read);
        }
        return TRUE;
    } else
        return FALSE;
}

// INT pin of i2c PCA9672 GPIO expander is connected to FT4222 GPIO #3
// We use INT to detect any expander GPIO level change
void*
ft4222_gpio_monitor(void* arg)
{
    Ftdi_4222_Shim* shim = static_cast<Ftdi_4222_Shim*>(arg);

    uint16_t prev_value = 0;
    ft4222_i2c_read_io_expander(*shim, &prev_value);
    while (!shim->gpio_mon.should_stop) {
        {
            lock_guard lock(shim->mtx_ft4222);
            if (ft4222_has_internal_gpio_triggered(*shim, GPIO_PORT_IO_INT)) {
                uint16_t value;
                if (ft4222_i2c_read_io_expander(*shim, &value) == MRAA_SUCCESS) {
                    uint16_t change_value = prev_value ^ value;
                    int i;
                    for (i = 0; i < MAX_IO_EXPANDER_PINS; ++i) {
                        uint16_t mask = 1 << i;
                        shim->gpio_mon.is_interrupt_detected[i] = (change_value & mask);
                    }
                    prev_value = value;
                }
            }
        }
        ft4222_sleep_ms(10);
    }
    return NULL;
}

void
ft4222_gpio_monitor_add_pin(Ftdi_4222_Shim& shim)
{
    if (shim.gpio_mon.num_active_pins == 0) {
        pthread_mutexattr_t attr_rec;
        pthread_mutexattr_init(&attr_rec);
        pthread_mutexattr_settype(&attr_rec, PTHREAD_MUTEX_RECURSIVE);
        if (pthread_mutex_init(&shim.gpio_mon.mutex, &attr_rec) != 0) {
            syslog(LOG_ERR, "Failed to setup GPIO monitor mutex for FT4222 access");
            throw std::runtime_error("Failed to setup GPIO monitor mutex for FT4222 access");
        }

        pthread_create(&shim.gpio_mon.thread, NULL, ft4222_gpio_monitor, &shim);
    }
    shim.gpio_mon.num_active_pins++;
}

void
ft4222_gpio_monitor_remove_pin(Ftdi_4222_Shim& shim)
{
    shim.gpio_mon.num_active_pins--;
    if (shim.gpio_mon.num_active_pins == 0) {
        shim.gpio_mon.should_stop = TRUE;
        pthread_join(shim.gpio_mon.thread, NULL);
        pthread_mutex_destroy(&shim.gpio_mon.mutex);
    }
}

mraa_boolean_t
ft4222_gpio_monitor_is_interrupt_detected(Ftdi_4222_Shim& shim, int pin)
{
    mraa_boolean_t is_interrupt_detected = FALSE;
    if (shim.gpio_mon.is_interrupt_detected[pin]) {
        shim.gpio_mon.is_interrupt_detected[pin] = FALSE;
        is_interrupt_detected = TRUE;
    }
    return is_interrupt_detected;
}

mraa_result_t
gpio_interrupt_handler_init_replace(mraa_gpio_context dev)
{
    while (dev->isr_thread_terminating) {
        ft4222_sleep_ms(10);
    }

    Ftdi_4222_Shim* shim = ShimFromGpioPin(dev->phy_pin);
    if (!shim)
        return MRAA_ERROR_NO_RESOURCES;

    lock_guard lock(shim->mtx_ft4222);

    std::string extra;
    switch (ft4222_get_gpio_type(dev->phy_pin)) {
        case GPIO_TYPE_BUILTIN:
            /* Make sure this pin is an input */
            // ftdi_ft4222_set_internal_gpio_dir(*shim, dev->phy_pin, GPIO_INPUT);
            extra += "(FT4222 GPIO pin)";
            break;
        case GPIO_TYPE_PCA9672:
        case GPIO_TYPE_PCA9555:
            /* Make sure pin is an input */
            ftdi_ft4222_set_internal_gpio_dir(*shim, GPIO_PORT_IO_INT, GPIO_INPUT);
            ftdi_ft4222_set_internal_gpio_trigger(GPIO_PORT_IO_INT, GPIO_TRIGGER_FALLING);
            ft4222_gpio_monitor_add_pin(*shim);
            extra += "(FT4222 expander GPIO pin)";
            break;
        default:
            return MRAA_ERROR_INVALID_RESOURCE;
    }
    syslog(LOG_NOTICE, "ISR added for pin: %d physical_pin: %d %s", dev->pin, dev->phy_pin, extra.c_str());

    return MRAA_SUCCESS;
}

/* This method is running in a cancelable thread which can go away at any time,
 * likewise, the isr_thread_terminating check might never happen and the GPIO
 * monitor can have problems - not sure of the best fix for this... most likely
 * the best fix would be to NOT support the I/O expanders. */
mraa_result_t
gpio_wait_interrupt_replace(mraa_gpio_context dev)
{

    Ftdi_4222_Shim* shim = ShimFromGpioPin(dev->phy_pin);
    if (!shim)
        return MRAA_ERROR_NO_RESOURCES;


    mraa_boolean_t interrupt_detected = FALSE;
    ft4222_gpio_type gpio_type = GPIO_TYPE_BUILTIN;

    {
        lock_guard lock(shim->mtx_ft4222);
        gpio_type = ft4222_get_gpio_type(dev->phy_pin);
    }

    while (!dev->isr_thread_terminating && !interrupt_detected) {

        switch (gpio_type) {
            case GPIO_TYPE_BUILTIN: {
                lock_guard lock(shim->mtx_ft4222);
                interrupt_detected = ft4222_has_internal_gpio_triggered(*shim, dev->phy_pin);
            } break;
            case GPIO_TYPE_PCA9672:
            case GPIO_TYPE_PCA9555: {
                lock_guard lock(shim->mtx_ft4222);
                /* Expander pin indexing starts past the FT4222 GPIO pins */
                interrupt_detected =
                ft4222_gpio_monitor_is_interrupt_detected(*shim, dev->phy_pin - gpioPinsPerFt4222);
            } break;
            default:;
        }
        ft4222_sleep_ms(10);
    }
    if (dev->isr_thread_terminating) {
        lock_guard lock(shim->mtx_ft4222);
        ft4222_gpio_monitor_remove_pin(*shim);
    }

    return MRAA_SUCCESS;
}

void
ft4222_populate_i2c_func_table(mraa_adv_func_t* func_table)
{
    func_table->i2c_init_bus_replace = &i2c_init_bus_replace;
    func_table->i2c_set_frequency_replace = &i2c_set_frequency_replace;
    func_table->i2c_address_replace = &i2c_address_replace;
    func_table->i2c_read_replace = &i2c_read_replace;
    func_table->i2c_read_byte_replace = &i2c_read_byte_replace;
    func_table->i2c_read_byte_data_replace = &i2c_read_byte_data_replace;
    func_table->i2c_read_word_data_replace = &i2c_read_word_data_replace;
    func_table->i2c_read_bytes_data_replace = &i2c_read_bytes_data_replace;
    func_table->i2c_write_replace = &i2c_write_replace;           // Used in 3 places
    func_table->i2c_write_byte_replace = &i2c_write_byte_replace; // No mutex needed
    func_table->i2c_write_byte_data_replace = &i2c_write_byte_data_replace;
    func_table->i2c_write_word_data_replace = &i2c_write_word_data_replace;
    func_table->i2c_stop_replace = &i2c_stop_replace;
}

void
ft4222_populate_gpio_func_table(mraa_adv_func_t* func_table)
{
    func_table->gpio_init_internal_replace = &gpio_init_internal_replace;
    func_table->gpio_mode_replace = &gpio_mode_replace;
    func_table->gpio_edge_mode_replace = &gpio_edge_mode_replace;
    func_table->gpio_dir_replace = &gpio_dir_replace;
    func_table->gpio_read_replace = &gpio_read_replace;
    func_table->gpio_write_replace = &gpio_write_replace; // 6
    func_table->gpio_interrupt_handler_init_replace = &gpio_interrupt_handler_init_replace;
    func_table->gpio_wait_interrupt_replace = &gpio_wait_interrupt_replace;
}

/* Store mraa_board_t subplatform allocated by this library */
char ftdi_4222_platform_name[] = "FTDI FT4222";
bool
Ftdi_4222_Shim::setup_io()
{
    int numI2cGpioExpanderPins = _detect_io_expander();
    int numUsbGpio = gpioPinsPerFt4222 + numI2cGpioExpanderPins;
    int numI2cBusses = 1 + ft4222_detect_i2c_switch();
    int numUsbPins = numUsbGpio + 2 * (numI2cBusses - 1); // Add SDA and SCL for each i2c switch bus
    mraa_pincapabilities_t pinCapsI2c = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
    mraa_pincapabilities_t pinCapsI2cGpio = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 1, 0, 0 };
    mraa_pincapabilities_t pinCapsGpio = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };

    _board.platform_name = &ftdi_4222_platform_name[0];
    _board.platform_type = MRAA_FTDI_FT4222;
    _board.phy_pin_count = numUsbPins;
    _board.gpio_count = numUsbGpio;

    int bus = 0;
    _board.i2c_bus_count = numI2cBusses;
    _board.def_i2c_bus = bus;
    _board.i2c_bus[bus].bus_id = bus;

    /* Save off an I2C bus-to-shim value */
    _i2c_bus_to_shim[bus] = this;

    // I2c pins (these are virtual, entries are required to configure i2c layer)
    // We currently assume that GPIO 0/1 are reserved for i2c operation
    _pins.push_back(mraa_pininfo_t());
    strncpy(&_pins.back().name[0], "IGPIO0/SCL0", MRAA_PIN_NAME_SIZE);
    _pins.back().capabilities = pinCapsI2cGpio;
    _pins.back().gpio.pinmap = 512 + _pins.size() - 1;
    _pins.back().gpio.mux_total = 0;
    _pins.back().i2c.mux_total = 0;
    _board.i2c_bus[bus].scl = _pins.size() - 1;
    /* Save off a GPIO ndx-to-shim value */
    _gpio_pin_to_shim[_pins.size() - 1] = this;

    _pins.push_back(mraa_pininfo_t());
    strncpy(&_pins.back().name[0], "IGPIO1/SDA0", MRAA_PIN_NAME_SIZE);
    _pins.back().capabilities = pinCapsI2cGpio;
    _pins.back().gpio.pinmap = 512 + _pins.size() - 1;
    _pins.back().gpio.mux_total = 0;
    _pins.back().i2c.mux_total = 0;
    _board.i2c_bus[bus].sda = _pins.size() - 1;
    /* Save off a GPIO ndx-to-shim value */
    _gpio_pin_to_shim[_pins.size() - 1] = this;

    // FTDI4222 gpio
    _pins.push_back(mraa_pininfo_t());
    strncpy(&_pins.back().name[0], "INT-GPIO2", MRAA_PIN_NAME_SIZE);
    _pins.back().capabilities = pinCapsGpio;
    _pins.back().gpio.pinmap = 512 + _pins.size() - 1;
    _pins.back().gpio.mux_total = 0;
    /* Save off a GPIO ndx-to-shim value */
    _gpio_pin_to_shim[_pins.size() - 1] = this;

    _pins.push_back(mraa_pininfo_t());
    strncpy(&_pins.back().name[0], "INT-GPIO3", MRAA_PIN_NAME_SIZE);
    _pins.back().capabilities = pinCapsGpio;
    _pins.back().gpio.pinmap = 512 + _pins.size() - 1;
    _pins.back().gpio.mux_total = 0;
    /* Save off a GPIO ndx-to-shim value */
    _gpio_pin_to_shim[_pins.size() - 1] = this;


    // Virtual gpio pins on i2c I/O expander.
    for (int i = 0; i < numI2cGpioExpanderPins; ++i) {
        _pins.push_back(mraa_pininfo_t());
        snprintf(&_pins.back().name[0], MRAA_PIN_NAME_SIZE, "EXP-GPIO%d", i);
        _pins.back().capabilities = pinCapsGpio;
        _pins.back().gpio.pinmap = 512 + _pins.size() - 1;
        _pins.back().gpio.mux_total = 0;
        /* Save off a GPIO ndx-to-shim value */
        _gpio_pin_to_shim[_pins.size() - 1] = this;
    }

    // Now add any extra i2c buses behind i2c switch
    for (bus = 1; bus < numI2cBusses; ++bus) {
        _board.i2c_bus[bus].bus_id = bus;
        _pins.back().i2c.mux_total = 0;
        snprintf(&_pins.back().name[0], MRAA_PIN_NAME_SIZE, "SDA%d", bus);
        _pins.back().capabilities = pinCapsI2c;
        _board.i2c_bus[bus].sda = _pins.size() - 1;

        _pins.push_back(mraa_pininfo_t());
        snprintf(&_pins.back().name[0], MRAA_PIN_NAME_SIZE, "SCL%d", bus);
        _pins.back().capabilities = pinCapsI2c;
        _pins.back().i2c.mux_total = 0;
        _board.i2c_bus[bus].scl = _pins.size() - 1;

        _pins.push_back(mraa_pininfo_t());
    }
    /* The board pins points to the shim pins vector */
    _board.pins = &_pins[0];

    _board.adv_func = &_adv_func_table;

    ft4222_populate_i2c_func_table(_board.adv_func);
    ft4222_populate_gpio_func_table(_board.adv_func);

    /* Success, return the sub platform */
    return true;
}

bool
Ftdi_4222_Shim::init_ftdi_gpios()
{
    /* Setting the direction requires a re-init, which means the
     * previous resources should also be un-initialized first */
    if (h_gpio)
        FT4222_UnInitialize(h_gpio);

    /* Use GPIO2 as GPIO (not suspend output) */
    return (FT4222_SetSuspendOut(h_gpio, 0) == FT4222_OK) &&
           /* Use GPIO3 as GPIO (not interrupt for I/O expander) */
           (FT4222_SetWakeUpInterrupt(h_gpio, 0) == FT4222_OK) &&
           (FT4222_GPIO_Init(h_gpio, &pinDirection[0]) == FT4222_OK);
}

} /* namespace ft4222 */

/* One C method exposed for loading a platform */
mraa_result_t
mraa_usb_platform_extender(mraa_board_t* board)
{
    /* If no board provided return unknown */
    if (board == NULL)
        return MRAA_ERROR_PLATFORM_NOT_INITIALISED;

    /* Attempt to initialize an FT4222 shim and assign it as a sub-platform */
    board->sub_platform = ft4222::Ftdi_4222_Shim::init_and_setup_mraa_board();

    /* No sub-platform returned, drop out */
    if (board->sub_platform == NULL)
        return MRAA_ERROR_PLATFORM_NOT_INITIALISED;

    syslog(LOG_NOTICE, "Added subplatform of type: %s", board->sub_platform->platform_name);

    return MRAA_SUCCESS;
}
