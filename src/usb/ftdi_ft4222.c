/*
 * Author: Henry Bruce <henry.bruce@intel.com>
 * Copyright (c) 2015 Intel Corporation.
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

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include "linux/i2c-dev.h"
#include "common.h"
#include "ftd2xx.h"
#include "libft4222.h"
#include "usb/ftdi_ft4222.h"

#define PLATFORM_NAME "FTDI FT4222"
#define I2CM_ERROR(status) (((status) &0x02) != 0)
#define PCA9672_ADDR 0x20
#define PCA9545_ADDR 0x70
#define PCA9672_PINS 8
#define PCA9545_BUSSES 4
#define GPIO_PORT_IO_RESET GPIO_PORT2
#define GPIO_PORT_IO_STATUS GPIO_PORT3

static FT_HANDLE ftHandleGpio = (FT_HANDLE) NULL; //GPIO Handle
static FT_HANDLE ftHandleI2c = (FT_HANDLE) NULL; //I2C/SPI Handle
static FT_HANDLE ftHandleSpi = (FT_HANDLE) NULL; //I2C/SPI Handle
static GPIO_Dir pinDirection[] = {GPIO_OUTPUT, GPIO_OUTPUT, GPIO_OUTPUT, GPIO_OUTPUT};
static int bus_speed = 400;
static int numFt4222GpioPins = 4;
static int numI2cGpioExpanderPins = 8;
static int numI2cSwitchBusses = 4;
static int currentI2cBus = 0;

mraa_result_t
mraa_ftdi_ft4222_init()
{
    mraa_result_t mraaStatus = MRAA_ERROR_NO_RESOURCES;
    FT_DEVICE_LIST_INFO_NODE* devInfo = NULL;
    FT_STATUS ftStatus;
    DWORD numDevs = 0;
    int i;
    int retCode = 0;

    ftStatus = FT_CreateDeviceInfoList(&numDevs);
    if (ftStatus != FT_OK) {
        syslog(LOG_ERR, "FT_CreateDeviceInfoList failed: error code %d\n", ftStatus);
        goto init_exit;
    }

    devInfo = calloc((size_t) numDevs, sizeof(FT_DEVICE_LIST_INFO_NODE));
    if (devInfo == NULL) {
        syslog(LOG_ERR, "FT4222 allocation failure.\n");
        goto init_exit;
    }

    ftStatus = FT_GetDeviceInfoList(devInfo, &numDevs);
    syslog(LOG_NOTICE, "FT_GetDeviceInfoList returned %d devices\n", numDevs);
    if (ftStatus != FT_OK) {
        syslog(LOG_ERR, "FT_GetDeviceInfoList failed (error code %d)\n", (int) ftStatus);
        goto init_exit;
    }
    if (numDevs < 2) {
        syslog(LOG_ERR, "No FT4222 devices connected.\n");
        goto init_exit;
    }
    if(numDevs > 2) {
        syslog(LOG_ERR, "CNFMODE not supported. Valid modes are 0 or 3.\n");
        goto init_exit;
    }

    // FIXME: Assumes just one physical FTDI device present
    DWORD locationIdI2c = 0;
    DWORD locationIdGpio = 0;
    if (devInfo[0].Type == FT_DEVICE_4222H_0)
            locationIdI2c = devInfo[0].LocId;
    if (devInfo[1].Type == FT_DEVICE_4222H_0)
            locationIdGpio = devInfo[1].LocId;

    if (locationIdI2c == 0) {
        syslog(LOG_ERR, "FT_GetDeviceInfoList contains no I2C controllers\n");
        goto init_exit;
    }

    if (locationIdGpio == 0) {
        syslog(LOG_ERR, "FT_GetDeviceInfoList contains no GPIO controllers\n");
        goto init_exit;
    }

    ftStatus = FT_OpenEx((PVOID)(uintptr_t) locationIdI2c, FT_OPEN_BY_LOCATION, &ftHandleI2c);
    if (ftStatus != FT_OK) {
        syslog(LOG_ERR, "FT_OpenEx failed (error %d)\n", (int) ftStatus);
        goto init_exit;
    }

    ftStatus = FT_OpenEx((PVOID)(uintptr_t) locationIdGpio, FT_OPEN_BY_LOCATION, &ftHandleGpio);
    if (ftStatus != FT_OK) {
        syslog(LOG_ERR, "FT_OpenEx failed (error %d)\n", (int) ftStatus);
        goto init_exit;
    }

    FT4222_SetSuspendOut(ftHandleGpio, 0);
    FT4222_SetWakeUpInterrupt(ftHandleGpio, 0);
    ftStatus =  FT4222_GPIO_Init(ftHandleGpio, pinDirection);
    if (ftStatus != FT_OK) {
        syslog(LOG_ERR, "FT4222_GPIO_Init failed (error %d)\n", (int) ftStatus);
        mraaStatus = MRAA_ERROR_NO_RESOURCES;
        goto init_exit;
    }

    // Tell the FT4222 to be an I2C Master by default on init.
    FT4222_STATUS ft4222Status = FT4222_I2CMaster_Init(ftHandleI2c, bus_speed);
    if (FT4222_OK != ft4222Status) {
        syslog(LOG_ERR, "FT4222_I2CMaster_Init failed (error %d)!\n", ft4222Status);
        goto init_exit;
    }

    ft4222Status = FT4222_I2CMaster_Reset(ftHandleI2c);
    if (FT4222_OK != ft4222Status) {
        syslog(LOG_ERR, "FT4222_I2CMaster_Reset failed (error %d)!\n", ft4222Status);
        goto init_exit;
    }

    mraaStatus = MRAA_SUCCESS;

init_exit:
    if (devInfo != NULL)
        free(devInfo);
    if (mraaStatus == MRAA_SUCCESS)
        syslog(LOG_NOTICE, "mraa_ftdi_ft4222_init completed successfully\n");
    return mraaStatus;
}


mraa_result_t
mraa_ftdi_ft4222_get_version(unsigned int* versionChip, unsigned int* versionLib)
{
    if (ftHandleI2c != NULL) {
        FT4222_Version ft4222Version;
        FT4222_STATUS ft4222Status = FT4222_GetVersion(ftHandleI2c, &ft4222Version);
        if (FT4222_OK == ft4222Status) {
            *versionChip = (unsigned int) ft4222Version.chipVersion;
            *versionLib = (unsigned int) ft4222Version.dllVersion;
            syslog(LOG_NOTICE, "FT4222_GetVersion %08X %08X\n", *versionChip, *versionLib);
            return MRAA_SUCCESS;
        } else {
            syslog(LOG_ERR, "FT4222_GetVersion failed (error %d)\n", (int) ft4222Status);
            return MRAA_ERROR_NO_RESOURCES;
        }
    } else {
        syslog(LOG_ERR, "Bad FT4222 handle\n");
        return MRAA_ERROR_INVALID_HANDLE;
    }
}


/******************* Private I2C functions *******************/

static int
mraa_ftdi_ft4222_i2c_read_internal(FT_HANDLE handle, uint8_t addr, uint8_t* data, int length)
{
    uint16 bytesRead = 0;
    uint8 controllerStatus;
    // syslog(LOG_NOTICE, "FT4222_I2CMaster_Read(%#02X, %#02X)", addr, length);
    FT4222_STATUS ft4222Status = FT4222_I2CMaster_Read(handle, addr, data, length, &bytesRead);
    ft4222Status = FT4222_I2CMaster_GetStatus(ftHandleI2c, &controllerStatus);
    if (FT4222_OK != ft4222Status || I2CM_ERROR(controllerStatus)) {
        syslog(LOG_ERR, "FT4222_I2CMaster_Read failed for address %#02x\n", addr);
        FT4222_I2CMaster_Reset(handle);
        return 0;
    }
    return bytesRead;
}

static int
mraa_ftdi_ft4222_i2c_write_internal(FT_HANDLE handle, uint8_t addr, const uint8_t* data, int bytesToWrite)
{
    uint16 bytesWritten = 0;
    uint8 controllerStatus;
    // syslog(LOG_NOTICE, "FT4222_I2CMaster_Write(%#02X, %#02X, %d)", addr, *data, bytesToWrite);
    FT4222_STATUS ft4222Status = FT4222_I2CMaster_Write(handle, addr, (uint8_t*) data, bytesToWrite, &bytesWritten);
    ft4222Status = FT4222_I2CMaster_GetStatus(ftHandleI2c, &controllerStatus);
    if (FT4222_OK != ft4222Status || I2CM_ERROR(controllerStatus)) {
        syslog(LOG_ERR, "FT4222_I2CMaster_Write failed address %#02x\n", addr);
        FT4222_I2CMaster_Reset(handle);
        return 0;
    }

    if (bytesWritten != bytesToWrite)
        syslog(LOG_ERR, "FT4222_I2CMaster_Write wrote %u of %u bytes.\n", bytesWritten, bytesToWrite);

    return bytesWritten;
}


// Function detects known I2C I/O expanders and returns the number of GPIO pins on expander
static int
mraa_ftdi_ft4222_detect_io_expander()
{
    uint8_t data;
    if(mraa_ftdi_ft4222_i2c_read_internal(ftHandleI2c, PCA9672_ADDR, &data, 1) == 1) {
        return PCA9672_PINS;
    }
    return 0;
}


static mraa_boolean_t
mraa_ftdi_ft4222_is_internal_gpio(int pin)
{
    return pin < numFt4222GpioPins;
}


static mraa_result_t
ftdi_ft4222_set_internal_gpio_dir(int pin, GPIO_Dir direction)
{
    pinDirection[pin] = direction;
    if (FT4222_GPIO_Init(ftHandleGpio, pinDirection) != FT4222_OK)
        return MRAA_ERROR_UNSPECIFIED;
    else
        return MRAA_SUCCESS;
}

// Function detects known I2C switches and returns the number of busses.
// On startup switch is disabled so default bus will be integrated i2c bus.
static int
mraa_ftdi_ft4222_detect_i2c_switch()
{
    uint8_t data;
    if(mraa_ftdi_ft4222_i2c_read_internal(ftHandleI2c, PCA9545_ADDR, &data, 1) == 1) {
        data = 0;
        return mraa_ftdi_ft4222_i2c_write_internal(ftHandleI2c, PCA9545_ADDR, &data, 1) == 1 ? PCA9545_BUSSES : 0;
    }
    return 0;
}


static mraa_result_t
mraa_ftdi_ft4222_i2c_select_bus(int bus)
{
    if (bus != currentI2cBus) {
        uint8_t data;
        if (bus == 0)
            data = 0;
        else
            data = 1 << (bus-1);
        if (mraa_ftdi_ft4222_i2c_write_internal(ftHandleI2c, PCA9545_ADDR, &data, 1) == 1)
            currentI2cBus = bus;
        else
            return MRAA_ERROR_UNSPECIFIED;
    }
    return MRAA_SUCCESS;
}

static int
mraa_ftdi_ft4222_i2c_context_read(mraa_i2c_context dev, uint8_t* data, int length)
{
    if (mraa_ftdi_ft4222_i2c_select_bus(dev->busnum) == MRAA_SUCCESS)
        return mraa_ftdi_ft4222_i2c_read_internal(dev->handle, dev->addr, data, length);
    else
        return 0;
}

static int
mraa_ftdi_ft4222_i2c_context_write(mraa_i2c_context dev, uint8_t* data, int length)
{
    if (mraa_ftdi_ft4222_i2c_select_bus(dev->busnum) == MRAA_SUCCESS)
        return mraa_ftdi_ft4222_i2c_write_internal(dev->handle, dev->addr, data, length);
    else
        return 0;
}


static void
mraa_ftdi_ft4222_sleep_ms(unsigned long mseconds)
{
    struct timespec sleepTime;

    sleepTime.tv_sec = mseconds / 1000;              // Number of seconds
    sleepTime.tv_nsec = (mseconds % 1000) * 1000000; // Convert fractional seconds to nanoseconds

    // Iterate nanosleep in a loop until the total sleep time is the original
    // value of the seconds parameter
    while ((nanosleep(&sleepTime, &sleepTime) != 0) && (errno == EINTR))
        ;
}

static unsigned int
mraa_ftdi_ft4222_get_tick_count_ms()
{
    static unsigned int startTick = 0;
    unsigned int ticks;
    struct timeval now;
    gettimeofday(&now, NULL);
    ticks = now.tv_sec * 1000;
    ticks += now.tv_usec / 1000;
    if (startTick == 0)
        startTick = ticks;
    return ticks - startTick;
}


/******************* I2C functions *******************/

static mraa_result_t
mraa_ftdi_ft4222_i2c_init_bus_replace(mraa_i2c_context dev)
{
    // Tell the FT4222 to be an I2C Master.
    FT4222_STATUS ft4222Status = FT4222_I2CMaster_Init(ftHandleI2c, bus_speed);
    if (FT4222_OK != ft4222Status) {
        syslog(LOG_ERR, "FT4222_I2CMaster_Init failed (error %d)!\n", ft4222Status);
        return MRAA_ERROR_NO_RESOURCES;
    }

    // Reset the I2CM registers to a known state.
    ft4222Status = FT4222_I2CMaster_Reset(ftHandleI2c);
    if (FT4222_OK != ft4222Status) {
        syslog(LOG_ERR, "FT4222_I2CMaster_Reset failed (error %d)!\n", ft4222Status);
        return MRAA_ERROR_NO_RESOURCES;
    }

    syslog(LOG_NOTICE, "I2C interface enabled GPIO0 and GPIO1 will be unavailable.\n");
    dev->handle = ftHandleI2c;
    dev->fh = -1;              // We don't use file descriptors
    dev->funcs = I2C_FUNC_I2C; // Advertise minimal i2c support as per
                               // https://www.kernel.org/doc/Documentation/i2c/functionality
    return MRAA_SUCCESS;
}


static mraa_result_t
mraa_ftdi_ft4222_i2c_frequency(mraa_i2c_context dev, mraa_i2c_mode_t mode)
{
    switch (mode) {
        case MRAA_I2C_STD: /**< up to 100Khz */
            bus_speed = 100;
            break;
        MRAA_I2C_FAST: /**< up to 400Khz */
            bus_speed = 400;
            break;
        MRAA_I2C_HIGH: /**< up to 3.4Mhz */
            bus_speed = 3400;
            break;
    }
    return FT4222_I2CMaster_Init(ftHandleI2c, bus_speed) == FT4222_OK ? MRAA_SUCCESS : MRAA_ERROR_NO_RESOURCES;
}


static mraa_result_t
mraa_ftdi_ft4222_i2c_address(mraa_i2c_context dev, uint8_t addr)
{
    dev->addr = (int) addr;
    return MRAA_SUCCESS;
}


static int
mraa_ftdi_ft4222_i2c_read(mraa_i2c_context dev, uint8_t* data, int length)
{
    return mraa_ftdi_ft4222_i2c_read_internal(dev->handle, dev->addr, data, length);
}

static uint8_t
mraa_ftdi_ft4222_i2c_read_byte(mraa_i2c_context dev)
{
    uint8_t data;
    if (mraa_ftdi_ft4222_i2c_context_read(dev, &data, 1) == 1)
        return data;
    else
        return 0;
}


static uint16_t
mraa_ftdi_ft4222_i2c_read_word_data(mraa_i2c_context dev, uint8_t command)
{
    uint8_t buf[2];
    uint16_t data;
    if (mraa_ftdi_ft4222_i2c_context_write(dev, &command, 1) != 1)
        return 0;
    if (mraa_ftdi_ft4222_i2c_context_read(dev, buf, 2) != 2)
        return 0;
    data = *(uint16_t*)buf;
    return data;
}

static int
mraa_ftdi_ft4222_i2c_read_bytes_data(mraa_i2c_context dev, uint8_t command, uint8_t* data, int length)
{
    if (mraa_ftdi_ft4222_i2c_context_write(dev, &command, 1) != 1)
        return 0;
    return mraa_ftdi_ft4222_i2c_context_read(dev, data, length);
}


static mraa_result_t
mraa_ftdi_ft4222_i2c_write(mraa_i2c_context dev, const uint8_t* data, int bytesToWrite)
{
    uint16 bytesWritten = mraa_ftdi_ft4222_i2c_context_write(dev, (uint8_t*)data, bytesToWrite);
    return bytesToWrite == bytesWritten ? MRAA_SUCCESS : MRAA_ERROR_INVALID_HANDLE;
}


static mraa_result_t
mraa_ftdi_ft4222_i2c_write_byte(mraa_i2c_context dev, uint8_t data)
{
    return mraa_ftdi_ft4222_i2c_write(dev, &data, 1);
}


static uint8_t
mraa_ftdi_ft4222_i2c_read_byte_data(mraa_i2c_context dev, uint8_t command)
{
    const uint8_t reg_addr = command;
    uint8_t data;
    if (mraa_ftdi_ft4222_i2c_write(dev, &reg_addr, 1) != MRAA_SUCCESS)
        return 0;
    if (mraa_ftdi_ft4222_i2c_read(dev, &data, 1) != 1)
        return 0;
    return data;
}

static mraa_result_t
mraa_ftdi_ft4222_i2c_write_byte_data(mraa_i2c_context dev, const uint8_t data, const uint8_t command)
{
    uint8_t buf[2];
    buf[0] = command;
    buf[1] = data;
    return mraa_ftdi_ft4222_i2c_write(dev, buf, 2);
}

static mraa_result_t
mraa_ftdi_ft4222_i2c_write_word_data(mraa_i2c_context dev, const uint16_t data, const uint8_t command)
{
    uint8_t buf[3];
    buf[0] = command;
    buf[1] = (uint8_t) data;
    buf[2] = (uint8_t)(data >> 8);
    return mraa_ftdi_ft4222_i2c_write(dev, buf, 3);
}

static mraa_result_t
mraa_ftdi_ft4222_i2c_stop(mraa_i2c_context dev)
{
    return MRAA_SUCCESS;
}

/******************* GPIO functions *******************/

static mraa_result_t
mraa_ftdi_ft4222_gpio_init_internal_replace(mraa_gpio_context dev, int pin)
{
    dev->phy_pin = (pin < numFt4222GpioPins) ? pin : pin - numFt4222GpioPins;
    if (pin < 2) {
        syslog(LOG_NOTICE, "Closing I2C interface to enable GPIO%d\n", pin);

        /* Replace with call to SPI init when SPI is fully implemented */
        FT4222_STATUS ft4222Status = FT4222_SPIMaster_Init(ftHandleSpi, SPI_IO_SINGLE, CLK_DIV_4, CLK_IDLE_HIGH, CLK_LEADING, 0x01);
        if (FT4222_OK != ft4222Status){
            syslog(LOG_ERR, "Failed to close I2C interface and start SPI (error %d)!\n", ft4222Status);
            return MRAA_ERROR_NO_RESOURCES;
        }
    }
    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_ftdi_ft4222_gpio_mode_replace(mraa_gpio_context dev, mraa_gpio_mode_t mode)
{
    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_ftdi_ft4222_gpio_edge_mode_replace(mraa_gpio_context dev, mraa_gpio_edge_t mode)
{
    return MRAA_SUCCESS;
}

static int
mraa_ftdi_ft4222_gpio_read_replace(mraa_gpio_context dev)
{
    uint8_t pin = dev->pin;
    if (mraa_ftdi_ft4222_is_internal_gpio(pin)) {
        // FTDI GPIO
        BOOL value;
        FT4222_STATUS ft4222Status = FT4222_GPIO_Read(ftHandleGpio, dev->phy_pin, &value);
        if (FT4222_OK != ft4222Status) {
            syslog(LOG_ERR, "FT4222_GPIO_Read failed (error %d)!\n", ft4222Status);
            return -1;
        }
        return value;
    }
    else {
        // Expander GPIO
        uint8_t mask = 1 << dev->phy_pin;
        uint8_t value;
        if (mraa_ftdi_ft4222_i2c_read_internal(ftHandleI2c, PCA9672_ADDR, &value, 1) != 1)
            return -1;
        return (value & mask) == mask;
    }
}


static mraa_result_t
mraa_ftdi_ft4222_gpio_write_replace(mraa_gpio_context dev, int write_value)
{
    uint8_t pin = dev->pin;
    if (mraa_ftdi_ft4222_is_internal_gpio(pin)) {
        // FTDI GPIO
        FT4222_STATUS ft4222Status = FT4222_GPIO_Write(ftHandleGpio, dev->phy_pin, write_value);
        if (FT4222_OK != ft4222Status) {
            syslog(LOG_ERR, "FT4222_GPIO_Write failed (error %d)!\n", ft4222Status);
            return MRAA_ERROR_UNSPECIFIED;
        }
    }
    else {
        // Expander GPIO
        uint8_t mask = 1 << dev->phy_pin;
        uint8_t value;
        if (mraa_ftdi_ft4222_i2c_read_internal(ftHandleI2c, PCA9672_ADDR, &value, 1) != 1)
            return MRAA_ERROR_UNSPECIFIED;
        if (write_value == 1)
            value |= mask;
        else
            value &= (~mask);
        if (mraa_ftdi_ft4222_i2c_write_internal(ftHandleI2c, PCA9672_ADDR, &value, 1) != 1)
            return MRAA_ERROR_UNSPECIFIED;
    }
    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_ftdi_ft4222_gpio_dir_replace(mraa_gpio_context dev, mraa_gpio_dir_t dir)
{
    switch (dir) {
        case MRAA_GPIO_IN:
            if (mraa_ftdi_ft4222_is_internal_gpio(dev->pin))
                return ftdi_ft4222_set_internal_gpio_dir(dev->phy_pin, GPIO_INPUT);
            else
                return mraa_ftdi_ft4222_gpio_write_replace(dev, 1);
        case MRAA_GPIO_OUT:
            if (mraa_ftdi_ft4222_is_internal_gpio(dev->pin))
                return ftdi_ft4222_set_internal_gpio_dir(dev->phy_pin, GPIO_OUTPUT);
            else
                return MRAA_SUCCESS;
        case MRAA_GPIO_OUT_HIGH:
            if (mraa_ftdi_ft4222_is_internal_gpio(dev->pin)) {
                if (ftdi_ft4222_set_internal_gpio_dir(dev->phy_pin, GPIO_OUTPUT) != MRAA_SUCCESS)
                    return MRAA_ERROR_UNSPECIFIED;
            }
            return mraa_ftdi_ft4222_gpio_write_replace(dev, 1);
        case MRAA_GPIO_OUT_LOW:
            if (mraa_ftdi_ft4222_is_internal_gpio(dev->pin)) {
                if (ftdi_ft4222_set_internal_gpio_dir(dev->phy_pin, GPIO_OUTPUT) != MRAA_SUCCESS)
                    return MRAA_ERROR_UNSPECIFIED;
            }
            return mraa_ftdi_ft4222_gpio_write_replace(dev, 0);
        default:
            return MRAA_ERROR_INVALID_PARAMETER;
    }
}


static void*
mraa_ftdi_ft4222_gpio_interrupt_handler_replace(mraa_gpio_context dev)
{
#ifdef USE_FT4222_GPIO_TRIGGER
    // FIXME: Use big buffer; shouldn't be more than this many events to read
    GPIO_Trigger event_buf[256];
    int prev_level = mraa_ftdi_ft4222_gpio_read_replace(dev);
    while (1) {
        uint16 num_events = 0;
        FT4222_STATUS status = FT4222_GPIO_GetTriggerStatus(ftHandleGpio, GPIO_PORT_IO_STATUS, &num_events);
        if (status != FT4222_OK)
            printf("FT4222_GPIO_GetTriggerStatus failed with code %d\n", status);
        printf("%u: FT4222_GPIO_GetTriggerStatus Events = %d\n", mraa_ftdi_ft4222_get_tick_count_ms(), num_events);
        if (num_events > 0) {
            int level = mraa_ftdi_ft4222_gpio_read_replace(dev);
            uint16 num_events_read;
            FT4222_GPIO_ReadTriggerQueue(ftHandleGpio, GPIO_PORT_IO_STATUS, event_buf, num_events, &num_events_read);
            // printf("%u: FT4222_GPIO_ReadTriggerQueue Events= %d\n", mraa_ftdi_ft4222_get_tick_count_ms(), num_events_read);
            printf("%u: level = %d\n", mraa_ftdi_ft4222_get_tick_count_ms(), level);
            if (level != prev_level) {
                dev->isr(dev->isr_args);
                prev_level = level;
            }

        }
        mraa_ftdi_ft4222_sleep_ms(20);
        // int level = mraa_ftdi_ft4222_gpio_read_replace(dev);
        // printf("level = %d\n", level);
    }
#else
    int prev_level = mraa_ftdi_ft4222_gpio_read_replace(dev);
    while (1) {
        int level = mraa_ftdi_ft4222_gpio_read_replace(dev);
        // MRAA_GPIO_EDGE_BOTH
        if (level != prev_level) {
            dev->isr(dev->isr_args);
            prev_level = level;
        }
        mraa_ftdi_ft4222_sleep_ms(100);
    }
#endif
    return NULL;
}

static void
mraa_ftdi_ft4222_populate_i2c_func_table(mraa_adv_func_t* func_table)
{
    func_table->i2c_init_bus_replace = &mraa_ftdi_ft4222_i2c_init_bus_replace;
    func_table->i2c_set_frequency_replace = &mraa_ftdi_ft4222_i2c_frequency;
    func_table->i2c_address_replace = &mraa_ftdi_ft4222_i2c_address;
    func_table->i2c_read_replace = &mraa_ftdi_ft4222_i2c_read;
    func_table->i2c_read_byte_replace = &mraa_ftdi_ft4222_i2c_read_byte;
    func_table->i2c_read_byte_data_replace = &mraa_ftdi_ft4222_i2c_read_byte_data;
    func_table->i2c_read_word_data_replace = &mraa_ftdi_ft4222_i2c_read_word_data;
    func_table->i2c_read_bytes_data_replace = &mraa_ftdi_ft4222_i2c_read_bytes_data;
    func_table->i2c_write_replace = &mraa_ftdi_ft4222_i2c_write;
    func_table->i2c_write_byte_replace = &mraa_ftdi_ft4222_i2c_write_byte;
    func_table->i2c_write_byte_data_replace = &mraa_ftdi_ft4222_i2c_write_byte_data;
    func_table->i2c_write_word_data_replace = &mraa_ftdi_ft4222_i2c_write_word_data;
    func_table->i2c_stop_replace = &mraa_ftdi_ft4222_i2c_stop;
}

static void
mraa_ftdi_ft4222_populate_gpio_func_table(mraa_adv_func_t* func_table)
{
    func_table->gpio_init_internal_replace = &mraa_ftdi_ft4222_gpio_init_internal_replace;
    func_table->gpio_mode_replace = &mraa_ftdi_ft4222_gpio_mode_replace;
    func_table->gpio_edge_mode_replace = &mraa_ftdi_ft4222_gpio_edge_mode_replace;
    func_table->gpio_dir_replace = &mraa_ftdi_ft4222_gpio_dir_replace;
    func_table->gpio_read_replace = &mraa_ftdi_ft4222_gpio_read_replace;
    func_table->gpio_write_replace = &mraa_ftdi_ft4222_gpio_write_replace;
    func_table->gpio_interrupt_handler_replace = &mraa_ftdi_ft4222_gpio_interrupt_handler_replace;
}


mraa_board_t*
mraa_ftdi_ft4222()
{
    mraa_board_t* sub_plat = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (sub_plat == NULL)
        return NULL;
    numI2cGpioExpanderPins = mraa_ftdi_ft4222_detect_io_expander();
    int pinIndex = 0;
    int numUsbGpio = numFt4222GpioPins + numI2cGpioExpanderPins;
    int numI2cBusses = 1 + mraa_ftdi_ft4222_detect_i2c_switch();
    int numUsbPins = numUsbGpio + 2 * (numI2cBusses-1); // Add SDA and SCL for each i2c switch bus
    mraa_pincapabilities_t pinCapsI2c = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
    mraa_pincapabilities_t pinCapsI2cGpio = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 1, 0, 0 };
    mraa_pincapabilities_t pinCapsGpio = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };

    sub_plat->platform_name = PLATFORM_NAME;
    sub_plat->phy_pin_count = numUsbPins;
    sub_plat->gpio_count = numUsbGpio;
    mraa_pininfo_t* pins = (mraa_pininfo_t*) calloc(numUsbPins,sizeof(mraa_pininfo_t));
    if (pins == NULL) {
        return NULL;
    }
    sub_plat->pins = pins;

    int bus = 0;
    sub_plat->i2c_bus_count = numI2cBusses;
    sub_plat->def_i2c_bus = bus;
    sub_plat->i2c_bus[bus].bus_id = bus;

    // I2c pins (these are virtual, entries are required to configure i2c layer)
    // We currently assume that GPIO 0/1 are reserved for i2c operation
    strncpy(sub_plat->pins[pinIndex].name, "IGPIO0/SCL0", MRAA_PIN_NAME_SIZE);
    sub_plat->pins[pinIndex].capabilites = pinCapsI2cGpio;
    sub_plat->pins[pinIndex].gpio.pinmap = pinIndex;
    sub_plat->pins[pinIndex].gpio.mux_total = 0;
    sub_plat->pins[pinIndex].i2c.mux_total = 0;
    sub_plat->i2c_bus[bus].scl = pinIndex;
    pinIndex++;
    strncpy(sub_plat->pins[pinIndex].name, "IGPIO1/SDA0", MRAA_PIN_NAME_SIZE);
    sub_plat->pins[pinIndex].capabilites = pinCapsI2cGpio;
    sub_plat->pins[pinIndex].gpio.pinmap = pinIndex;
    sub_plat->pins[pinIndex].gpio.mux_total = 0;
    sub_plat->pins[pinIndex].i2c.mux_total = 0;
    sub_plat->i2c_bus[bus].sda = pinIndex;
    pinIndex++;

    // FTDI4222 gpio
    strncpy(sub_plat->pins[pinIndex].name, "INT-GPIO2", MRAA_PIN_NAME_SIZE);
    sub_plat->pins[pinIndex].capabilites = pinCapsGpio;
    sub_plat->pins[pinIndex].gpio.pinmap = pinIndex;
    sub_plat->pins[pinIndex].gpio.mux_total = 0;
    pinIndex++;
    strncpy(sub_plat->pins[pinIndex].name, "INT-GPIO3", MRAA_PIN_NAME_SIZE);
    sub_plat->pins[pinIndex].capabilites = pinCapsGpio;
    sub_plat->pins[pinIndex].gpio.pinmap = pinIndex;
    sub_plat->pins[pinIndex].gpio.mux_total = 0;
    pinIndex++;

    // Virtual gpio pins on i2c I/O expander.
    int i;
    for (i = 0; i < numI2cGpioExpanderPins; ++i) {
        snprintf(sub_plat->pins[pinIndex].name, MRAA_PIN_NAME_SIZE, "EXP-GPIO%d", i);
        sub_plat->pins[pinIndex].capabilites = pinCapsGpio;
        sub_plat->pins[pinIndex].gpio.pinmap = pinIndex;
        sub_plat->pins[pinIndex].gpio.mux_total = 0;
        pinIndex++;
    }

    // Now add any extra i2c busses behind i2c switch
    for (bus = 1; bus < numI2cBusses; ++bus) {
        sub_plat->i2c_bus[bus].bus_id = bus;
        sub_plat->pins[pinIndex].i2c.mux_total = 0;
        snprintf(sub_plat->pins[pinIndex].name, MRAA_PIN_NAME_SIZE, "SDA%d", bus);
        sub_plat->pins[pinIndex].capabilites = pinCapsI2c;
        sub_plat->i2c_bus[bus].sda = pinIndex;
        pinIndex++;
        snprintf(sub_plat->pins[pinIndex].name, MRAA_PIN_NAME_SIZE, "SCL%d", bus);
        sub_plat->pins[pinIndex].capabilites = pinCapsI2c;
        sub_plat->pins[pinIndex].i2c.mux_total = 0;
        sub_plat->i2c_bus[bus].scl = pinIndex;
        pinIndex++;
    }


    // Set override functions
    mraa_adv_func_t* func_table = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (func_table == NULL) {
        return NULL;
    }
    mraa_ftdi_ft4222_populate_i2c_func_table(func_table);
    mraa_ftdi_ft4222_populate_gpio_func_table(func_table);

    sub_plat->adv_func = func_table;
    return sub_plat;
}
