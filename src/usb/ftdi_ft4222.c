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
#include <errno.h>
#include "linux/i2c-dev.h"
#include "common.h"
#include "ftd2xx.h"
#include "libft4222.h"
#include "usb/ftdi_ft4222.h"

#define PLATFORM_NAME "FTDI FT4222"
#define I2CM_ERROR(status) (((status) &0x02) != 0)
#define PCA9672_ADDR 0x20


static FT_HANDLE ftHandle = (FT_HANDLE) NULL;
static int bus_speed = 400;
static int numI2cGpioExapnderPins = 8;
static int numUsbGpio = 0;


mraa_result_t
mraa_ftdi_ft4222_init()
{
    mraa_result_t mraaStatus = MRAA_SUCCESS;
    FT_STATUS ftStatus;
    FT_DEVICE_LIST_INFO_NODE* devInfo = NULL;
    DWORD numDevs = 0;
    int i;
    int retCode = 0;

    ftStatus = FT_CreateDeviceInfoList(&numDevs);
    if (ftStatus != FT_OK) {
        syslog(LOG_ERR, "FT_CreateDeviceInfoList failed: error code %d\n", ftStatus);
        mraaStatus = MRAA_ERROR_NO_RESOURCES;
        goto init_exit;
    }

    if (numDevs == 0) {
        syslog(LOG_ERR, "No FT4222 devices connected.\n");
        goto init_exit;
    }

    devInfo = calloc((size_t) numDevs, sizeof(FT_DEVICE_LIST_INFO_NODE));
    if (devInfo == NULL) {
        syslog(LOG_ERR, "FT4222 allocation failure.\n");
        mraaStatus = MRAA_ERROR_NO_RESOURCES;
        goto init_exit;
    }

    ftStatus = FT_GetDeviceInfoList(devInfo, &numDevs);
    if (ftStatus != FT_OK) {
        syslog(LOG_ERR, "FT_GetDeviceInfoList failed (error code %d)\n", (int) ftStatus);
        mraaStatus = MRAA_ERROR_NO_RESOURCES;
        goto init_exit;
    }

    /*
            FT4222_Version ft4222Version;
            FT4222_STATUS ft4222Status = FT4222_GetVersion(ftHandle, &ft4222Version);
            if (FT4222_OK == ft4222Status){
                syslog(LOG_NOTICE, "FT4222_GetVersion %08X %08X\n", ft4222Version.chipVersion,
       ft4222Version.dllVersion);
             } else
                syslog(LOG_ERR, "FT4222_GetVersion failed with code %d", ft4222Status);
    */

    syslog(LOG_NOTICE, "FT_GetDeviceInfoList returned %d devices\n", numDevs);
    DWORD locationId = 0;
    for (i = 0; i < numDevs && locationId == 0; ++i) {
        // printf("%d: type = %d, location = %d\n", i, devInfo[i].Type, devInfo[i].LocId);
        if (devInfo[i].Type == FT_DEVICE_4222H_0 || devInfo[i].Type == FT_DEVICE_4222H_3)
            locationId = devInfo[i].LocId;
    }
    if (locationId == 0) {
        syslog(LOG_ERR, "FT_GetDeviceInfoList contains no valid devices\n");
        mraaStatus = MRAA_ERROR_NO_RESOURCES;
        goto init_exit;
    }

    ftStatus = FT_OpenEx((PVOID)(uintptr_t) locationId, FT_OPEN_BY_LOCATION, &ftHandle);
    if (ftStatus != FT_OK) {
        syslog(LOG_ERR, "FT_OpenEx failed (error %d)\n", (int) ftStatus);
        mraaStatus = MRAA_ERROR_NO_RESOURCES;
        goto init_exit;
    }

    // Tell the FT4222 to be an I2C Master.
    FT4222_STATUS ft4222Status = FT4222_I2CMaster_Init(ftHandle, bus_speed);
    if (FT4222_OK != ft4222Status) {
        syslog(LOG_ERR, "FT4222_I2CMaster_Init failed (error %d)!\n", ft4222Status);
        mraaStatus = MRAA_ERROR_NO_RESOURCES;
        goto init_exit;
    }

    // Reset the I2CM registers to a known state.
    ft4222Status = FT4222_I2CMaster_Reset(ftHandle);
    if (FT4222_OK != ft4222Status) {
        syslog(LOG_ERR, "FT4222_I2CMaster_Reset failed (error %d)!\n", ft4222Status);
        mraaStatus = MRAA_ERROR_NO_RESOURCES;
        goto init_exit;
    }


init_exit:
    if (devInfo != NULL)
        ;
    free(devInfo);
    if (mraaStatus == MRAA_SUCCESS)
        syslog(LOG_NOTICE, "mraa_ftdi_ft4222_init completed successfully\n");
    return mraaStatus;
}


mraa_result_t
mraa_ftdi_ft4222_get_version(unsigned int* versionChip, unsigned int* versionLib)
{
    if (ftHandle != NULL) {
        FT4222_Version ft4222Version;
        FT4222_STATUS ft4222Status = FT4222_GetVersion(ftHandle, &ft4222Version);
        if (FT4222_OK == ft4222Status) {
            *versionChip = (unsigned int) ft4222Version.chipVersion;
            *versionLib = (unsigned int) ft4222Version.dllVersion;
            syslog(LOG_NOTICE, "FT4222_GetVersion %08X %08X\n", *versionChip, *versionLib);
            return MRAA_SUCCESS;
        } else {
            syslog(LOG_ERR, "libmraa: FT4222_GetVersion failed (error %d)\n", (int) ft4222Status);
            return MRAA_ERROR_NO_RESOURCES;
        }
    } else {
        syslog(LOG_ERR, "libmraa: bad FT4222 handle\n");
        return MRAA_ERROR_INVALID_HANDLE;
    }
}


static int
mraa_ftdi_ft4222_i2c_read_internal(FT_HANDLE handle, uint8_t addr, uint8_t* data, int length)
{
    uint16 bytesRead = 0;
    uint8 controllerStatus;
    // syslog(LOG_NOTICE, "FT4222_I2CMaster_Read(%#02X, %#02X)", addr, length);
    FT4222_STATUS ft4222Status = FT4222_I2CMaster_Read(handle, addr, data, length, &bytesRead);
    ft4222Status = FT4222_I2CMaster_GetStatus(ftHandle, &controllerStatus);
    if (FT4222_OK != ft4222Status || I2CM_ERROR(controllerStatus)) {
        syslog(LOG_ERR, "FT4222_I2CMaster_Read failed (error %d)\n", (int) ft4222Status);
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
    FT4222_STATUS ft4222Status =
    FT4222_I2CMaster_Write(handle, addr, (uint8_t*) data, bytesToWrite, &bytesWritten);
    ft4222Status = FT4222_I2CMaster_GetStatus(ftHandle, &controllerStatus);
    if (FT4222_OK != ft4222Status || I2CM_ERROR(controllerStatus)) {
        syslog(LOG_ERR, "FT4222_I2CMaster_Write failed (error %d)\n", (int) ft4222Status);
        return 0;
    }

    if (bytesWritten != bytesToWrite)
        syslog(LOG_ERR, "FT4222_I2CMaster_Write wrote %u of %u bytes.\n", bytesWritten, bytesToWrite);

    return bytesWritten;
}


static mraa_boolean_t
mraa_ftdi_ft4222_detect_io_expander()
{
    uint8_t data;
    return mraa_ftdi_ft4222_i2c_read_internal(ftHandle, PCA9672_ADDR, &data, 1) == 1;
}


/******************* I2C functions *******************/
static mraa_i2c_context
mraa_ftdi_ft4222_i2c_init_raw(unsigned int bus)
{
    // Tell the FT4222 to be an I2C Master.
    FT4222_STATUS ft4222Status = FT4222_I2CMaster_Init(ftHandle, bus_speed);
    if (FT4222_OK != ft4222Status) {
        syslog(LOG_ERR, "FT4222_I2CMaster_Init failed (error %d)!\n", ft4222Status);
        return NULL;
    }

    // Reset the I2CM registers to a known state.
    ft4222Status = FT4222_I2CMaster_Reset(ftHandle);
    if (FT4222_OK != ft4222Status) {
        syslog(LOG_ERR, "FT4222_I2CMaster_Reset failed (error %d)!\n", ft4222Status);
        return NULL;
    }

    mraa_i2c_context dev = (mraa_i2c_context) malloc(sizeof(struct _i2c));
    if (dev == NULL) {
        syslog(LOG_CRIT, "i2c: Failed to allocate memory for context");
        return NULL;
    }

    dev->handle = ftHandle;
    dev->fh = -1;              // We don't use file descriptors
    dev->funcs = I2C_FUNC_I2C; // Advertise minimal i2c support as per
                               // https://www.kernel.org/doc/Documentation/i2c/functionality
    return dev;
}

static mraa_result_t
mraa_ftdi_ft4222_i2c_init_bus_replace(mraa_i2c_context dev)
{
    // Tell the FT4222 to be an I2C Master.
    FT4222_STATUS ft4222Status = FT4222_I2CMaster_Init(ftHandle, 400);
    if (FT4222_OK != ft4222Status) {
        syslog(LOG_ERR, "FT4222_I2CMaster_Init failed (error %d)!\n", ft4222Status);
        return MRAA_ERROR_NO_RESOURCES;
    }

    // Reset the I2CM registers to a known state.
    ft4222Status = FT4222_I2CMaster_Reset(ftHandle);
    if (FT4222_OK != ft4222Status) {
        syslog(LOG_ERR, "FT4222_I2CMaster_Reset failed (error %d)!\n", ft4222Status);
        return MRAA_ERROR_NO_RESOURCES;
    }

    dev->handle = ftHandle;
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
    return MRAA_SUCCESS;
}


static mraa_result_t
mraa_ftdi_ft4222_i2c_address(mraa_i2c_context dev, uint8_t addr)
{
    dev->addr = (int) addr;
    return FT4222_I2CMaster_Init(ftHandle, bus_speed) == FT4222_OK ? MRAA_SUCCESS : MRAA_ERROR_NO_RESOURCES;
}


static int
mraa_ftdi_ft4222_i2c_read(mraa_i2c_context dev, uint8_t* data, int length)
{
    return mraa_ftdi_ft4222_i2c_read_internal(dev->handle, dev->addr, data, length);
}

static uint8_t
mraa_ftdi_ft4222_i2c_read_byte(mraa_i2c_context dev)
{
    return 0;
}


static uint16_t
mraa_ftdi_ft4222_i2c_read_word_data(mraa_i2c_context dev, uint8_t command)
{
    return 0;
}

static int
mraa_ftdi_ft4222_i2c_read_bytes_data(mraa_i2c_context dev, uint8_t command, uint8_t* data, int length)
{
    return -1;
}


static mraa_result_t
mraa_ftdi_ft4222_i2c_write(mraa_i2c_context dev, const uint8_t* data, int bytesToWrite)
{
    uint16 bytesWritten = mraa_ftdi_ft4222_i2c_write_internal(dev->handle, dev->addr, data, bytesToWrite);
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
mraa_ftdi_ft4222_gpio_init_internal_replace(int pin)
{
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
    uint8_t pin = dev->phy_pin;
    uint8_t mask = 1 << pin;
    uint8_t value;
    if (mraa_ftdi_ft4222_i2c_read_internal(ftHandle, PCA9672_ADDR, &value, 1) != 1)
        return -1;
    return (value & mask) == mask;
}


static mraa_result_t
mraa_ftdi_ft4222_gpio_write_replace(mraa_gpio_context dev, int write_value)
{
    uint8_t pin = dev->phy_pin;
    uint8_t mask = 1 << pin;
    uint8_t value;
    if (mraa_ftdi_ft4222_i2c_read_internal(ftHandle, PCA9672_ADDR, &value, 1) != 1)
        return MRAA_ERROR_UNSPECIFIED;
    if (write_value == 1)
        value |= mask;
    else
        value &= (~mask);
    if (mraa_ftdi_ft4222_i2c_write_internal(ftHandle, PCA9672_ADDR, &value, 1) != 1)
        return MRAA_ERROR_UNSPECIFIED;
    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_ftdi_ft4222_gpio_dir_replace(mraa_gpio_context dev, mraa_gpio_dir_t dir)
{
    switch (dir) {
        case MRAA_GPIO_IN:
        case MRAA_GPIO_OUT_HIGH:
            return mraa_ftdi_ft4222_gpio_write_replace(dev, 1);
        case MRAA_GPIO_OUT_LOW:
            return mraa_ftdi_ft4222_gpio_write_replace(dev, 0);
        default:
            ;
    }
    return MRAA_SUCCESS;
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

static void*
mraa_ftdi_ft4222_gpio_interrupt_handler_replace(mraa_gpio_context dev)
{
    int prev_level = mraa_ftdi_ft4222_gpio_read_replace(dev);
    while (1) {
        int level = mraa_ftdi_ft4222_gpio_read_replace(dev);
        if (level != prev_level) {
            dev->isr(dev->isr_args);
            prev_level = level;
        }
        // printf("mraa_ftdi_ft4222_gpio_interrupt_handler_replace\n");
        mraa_ftdi_ft4222_sleep_ms(100);
    }
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
    mraa_boolean_t haveGpio = mraa_ftdi_ft4222_detect_io_expander();
    int pinIndex = 0;
    numUsbGpio = haveGpio ? numI2cGpioExapnderPins : 0;
    int numUsbPins = numUsbGpio + 2; // Add SDA and SCL
    sub_plat->platform_name = PLATFORM_NAME;
    sub_plat->phy_pin_count = numUsbPins;
    sub_plat->gpio_count = numUsbGpio;
    mraa_pininfo_t* pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * numUsbPins);
    if (pins == NULL) {
        return NULL;
    }
    sub_plat->pins = pins;

    // Virtual gpio pins on i2c I/O expander.
    mraa_pincapabilities_t pinCapsGpio = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    for (pinIndex = 0; pinIndex < numUsbGpio; ++pinIndex) {
        char name[8];
        sprintf(name, "Pin%d", pinIndex);
        strncpy(sub_plat->pins[pinIndex].name, name, 8);
        sub_plat->pins[pinIndex].capabilites = pinCapsGpio;
    }

    int bus = 0;
    sub_plat->i2c_bus_count = 1;
    sub_plat->def_i2c_bus = bus;
    sub_plat->i2c_bus[bus].bus_id = bus;

    // i2c pins (these are virtual, entries are required to configure i2c layer)
    mraa_pincapabilities_t pinCapsI2c = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
    strncpy(sub_plat->pins[pinIndex].name, "SDA", 8);
    sub_plat->pins[pinIndex].capabilites = pinCapsI2c;
    sub_plat->pins[pinIndex].i2c.mux_total = 0;
    sub_plat->i2c_bus[bus].sda = pinIndex;
    pinIndex++;
    strncpy(sub_plat->pins[pinIndex].name, "SCL", 8);
    sub_plat->pins[pinIndex].capabilites = pinCapsI2c;
    sub_plat->pins[pinIndex].i2c.mux_total = 0;
    sub_plat->i2c_bus[bus].scl = pinIndex;

    // Set override functions
    mraa_adv_func_t* func_table = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (func_table == NULL) {
        return NULL;
    }
    mraa_ftdi_ft4222_populate_i2c_func_table(func_table);
    if (haveGpio)
        mraa_ftdi_ft4222_populate_gpio_func_table(func_table);

    sub_plat->adv_func = func_table;
    return sub_plat;
}
