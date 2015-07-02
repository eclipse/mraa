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
#include "linux/i2c-dev.h"
#include "common.h"
#include "ftd2xx.h"
#include "libft4222.h"
#include "usb/ftdi_ft4222.h"

#define PLATFORM_NAME "FTDI FT4222"

extern mraa_board_t* plat;
extern mraa_func_t* mraa_drv_api_func_table;
static FT_HANDLE ftHandle = (FT_HANDLE)NULL;


mraa_result_t
mraa_ftdi_ft4222_init()
{
    mraa_result_t mraaStatus = MRAA_SUCCESS;
    FT_STATUS ftStatus;
    FT_DEVICE_LIST_INFO_NODE *devInfo = NULL;
    DWORD numDevs = 0;
    int i;
    int retCode = 0;
    
    ftStatus = FT_CreateDeviceInfoList(&numDevs);
    if (ftStatus != FT_OK) 
    {
        syslog(LOG_ERR, "FT_CreateDeviceInfoList failed: error code %d\n", ftStatus);
        mraaStatus = MRAA_ERROR_NO_RESOURCES;
        goto init_exit;
    }
    
    if (numDevs == 0)
    {
        syslog(LOG_ERR, "No FT4222 devices connected.\n");
        goto init_exit;
    }

    devInfo = calloc((size_t)numDevs, sizeof(FT_DEVICE_LIST_INFO_NODE));
    if (devInfo == NULL)
    {
        syslog(LOG_ERR, "FT4222 allocation failure.\n");
        mraaStatus = MRAA_ERROR_NO_RESOURCES;
        goto init_exit;
    }
    
    ftStatus = FT_GetDeviceInfoList(devInfo, &numDevs);
    if (ftStatus != FT_OK)
    {
        syslog(LOG_ERR, "FT_GetDeviceInfoList failed (error code %d)\n", (int)ftStatus);
        mraaStatus = MRAA_ERROR_NO_RESOURCES;
        goto init_exit;
    }

    syslog(LOG_INFO, "FT_GetDeviceInfoList returned %d devices\n", numDevs);
    DWORD locationId;
    int bus = 0;
    if (numDevs > bus && devInfo[bus].Type == FT_DEVICE_4222H_3) {
       locationId = devInfo[bus].LocId;
    } else {
        syslog(LOG_ERR, "FT_GetDeviceInfoList contains no valid devices\n");
        mraaStatus = MRAA_ERROR_NO_RESOURCES;
        goto init_exit;
    }

    ftStatus = FT_OpenEx((PVOID)(uintptr_t)locationId, FT_OPEN_BY_LOCATION, &ftHandle);
    if (ftStatus != FT_OK)
    {
        syslog(LOG_ERR, "FT_OpenEx failed (error %d)\n", (int)ftStatus);
        mraaStatus = MRAA_ERROR_NO_RESOURCES;
        goto init_exit;
    }

init_exit:
    if (devInfo != NULL);
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
        if (FT4222_OK == ft4222Status){
            *versionChip = (unsigned int)ft4222Version.chipVersion;
            *versionLib = (unsigned int)ft4222Version.dllVersion;
            syslog(LOG_NOTICE, "FT4222_GetVersion %08X %08X\n", *versionChip, *versionLib);
            return MRAA_SUCCESS;
        } else {
            syslog(LOG_ERR, "libmraa: FT4222_GetVersion failed (error %d)\n", (int)ft4222Status);
            return MRAA_ERROR_NO_RESOURCES;
        }
    } else {
        syslog(LOG_ERR, "libmraa: bad FT4222 handle\n");
        return MRAA_ERROR_INVALID_HANDLE;
    }
}


mraa_i2c_context
mraa_ftdi_ft4222_i2c_init_raw(unsigned int bus)
{
    // Tell the FT4222 to be an I2C Master.
    FT4222_STATUS ft4222Status = FT4222_I2CMaster_Init(ftHandle, 400);
    if (FT4222_OK != ft4222Status)
    {
        syslog(LOG_ERR, "FT4222_I2CMaster_Init failed (error %d)!\n", ft4222Status);
        return NULL;
    }
    
    // Reset the I2CM registers to a known state.
    ft4222Status = FT4222_I2CMaster_Reset(ftHandle);
    if (FT4222_OK != ft4222Status)
    {
        syslog(LOG_ERR, "FT4222_I2CMaster_Reset failed (error %d)!\n", ft4222Status);
        return NULL;
    }

    mraa_i2c_context dev = (mraa_i2c_context) malloc(sizeof(struct _i2c));
    if (dev == NULL) {
        syslog(LOG_CRIT, "i2c: Failed to allocate memory for context");
        return NULL;
    }

    dev->handle = ftHandle;
    dev->fh = -1; // We don't use file descriptors
    dev->funcs = I2C_FUNC_I2C; // Advertise minimal i2c support as per https://www.kernel.org/doc/Documentation/i2c/functionality
    return dev;
}

static mraa_result_t
mraa_ftdi_ft4222_i2c_frequency(mraa_i2c_context dev, mraa_i2c_mode_t mode)
{
    return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
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
    uint16 bytesRead = 0;
    syslog(LOG_NOTICE, "FT4222_I2CMaster_Read(%02X, %d)", dev->addr, length);
    FT4222_STATUS ft4222Status = FT4222_I2CMaster_Read(dev->handle, dev->addr, data, length, &bytesRead);
    if (FT4222_OK != ft4222Status)
    {
        syslog(LOG_ERR, "FT4222_I2CMaster_Read failed (error %d)\n", (int)ft4222Status);
        return 0;
    }
    return bytesRead;
}

static uint8_t
mraa_ftdi_ft4222_i2c_read_byte(mraa_i2c_context dev)
{
    return 0;
}

static uint8_t
mraa_ftdi_ft4222_i2c_read_byte_data(mraa_i2c_context dev, uint8_t command)
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
    uint16 bytesWritten = 0;
    syslog(LOG_NOTICE, "FT4222_I2CMaster_Write(%#02X, %#02X, %d)", dev->addr, *data, bytesToWrite);
    FT4222_STATUS ft4222Status = FT4222_I2CMaster_Write(dev->handle, dev->addr, (uint8_t*)data, bytesToWrite, &bytesWritten);
    if (FT4222_OK != ft4222Status)
    {
        syslog(LOG_ERR, "FT4222_I2CMaster_Write failed (error %d)\n", (int)ft4222Status);
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (bytesWritten != bytesToWrite)
        syslog(LOG_ERR, "FT4222_I2CMaster_Write wrote %u of %u bytes.\n", bytesWritten, bytesToWrite);

   return bytesToWrite == bytesWritten ? MRAA_SUCCESS : MRAA_ERROR_INVALID_HANDLE;
}


static mraa_result_t
mraa_ftdi_ft4222_i2c_write_byte(mraa_i2c_context dev, uint8_t data)
{
    return mraa_ftdi_ft4222_i2c_write(dev, &data, 1);
}

static mraa_result_t
mraa_ftdi_ft4222_i2c_write_byte_data(mraa_i2c_context dev, const uint8_t data, const uint8_t command)
{
    mraa_result_t status = mraa_ftdi_ft4222_i2c_write_byte(dev, command);
    if (status == MRAA_SUCCESS)
        return mraa_ftdi_ft4222_i2c_write_byte(dev, data);
    else
        return status;
}

static mraa_result_t
mraa_ftdi_ft4222_i2c_write_word_data(mraa_i2c_context dev, const uint16_t data, const uint8_t command)
{
    mraa_result_t status = mraa_ftdi_ft4222_i2c_write_byte(dev, command);
    if (status == MRAA_SUCCESS) 
        return mraa_ftdi_ft4222_i2c_write(dev, (const uint8_t*)&data, 2);
    else
        return status;

}

static mraa_result_t
mraa_ftdi_ft4222_i2c_stop(mraa_i2c_context dev)
{
    return MRAA_SUCCESS;
}




mraa_i2c_func_t*
mraa_i2c_ft4222_create_func_table()
{
    mraa_i2c_func_t* func_table = (mraa_i2c_func_t*) calloc(1, sizeof(mraa_i2c_func_t));
    if (func_table != NULL) {
        func_table->i2c_init_raw = &mraa_ftdi_ft4222_i2c_init_raw;
        func_table->i2c_frequency = &mraa_ftdi_ft4222_i2c_frequency;
        func_table->i2c_address = &mraa_ftdi_ft4222_i2c_address;
        func_table->i2c_read = &mraa_ftdi_ft4222_i2c_read;
        func_table->i2c_read_byte = &mraa_ftdi_ft4222_i2c_read_byte;
        func_table->i2c_read_byte_data = &mraa_ftdi_ft4222_i2c_read_byte_data;
        func_table->i2c_read_word_data = &mraa_ftdi_ft4222_i2c_read_word_data;
        func_table->i2c_read_bytes_data = &mraa_ftdi_ft4222_i2c_read_bytes_data;
        func_table->i2c_write = &mraa_ftdi_ft4222_i2c_write;
        func_table->i2c_write_byte = &mraa_ftdi_ft4222_i2c_write_byte;
        func_table->i2c_write_byte_data = &mraa_ftdi_ft4222_i2c_write_byte_data;
        func_table->i2c_write_word_data = &mraa_ftdi_ft4222_i2c_write_word_data;
        func_table->i2c_stop = &mraa_ftdi_ft4222_i2c_stop;    
    }
    return func_table;
}

mraa_board_t*
mraa_ftdi_ft4222()
{
    if (plat == NULL)
        return NULL;    
    int pinIndex = 0;
    int numUsbGpio = 0;
    int numUsbPins = numUsbGpio + 2; // Add SDA and SCL
    int numBasePins = plat->phy_pin_count;
    plat->platform_name = PLATFORM_NAME;
    mraa_pininfo_t* basePins = plat->pins;
    plat->phy_pin_count += numUsbPins;
    plat->gpio_count += numUsbGpio; 
    mraa_pininfo_t* baseAndExtendedPins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * plat->phy_pin_count);
    if (baseAndExtendedPins == NULL) {
        return NULL;
    }
    if (basePins != NULL) {
        memcpy(baseAndExtendedPins, basePins, sizeof(mraa_pininfo_t) * numBasePins);
        free(basePins);
    }
    plat->pins = baseAndExtendedPins;

    // Virtual gpio pins on i2c I/O expander.
    mraa_pincapabilities_t pinCapsGpio = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    for (pinIndex=numBasePins; pinIndex < numBasePins + numUsbGpio; ++pinIndex) {
        char name[8];
        sprintf(name, "Pin%d", pinIndex);
        strncpy(plat->pins[pinIndex].name, name, 8);
        plat->pins[pinIndex].capabilites = pinCapsGpio;
    }

    mraa_drv_api_t drv_type = MRAA_DRV_API_FT4222;
    int bus = 0;
    plat->i2c_bus_count = 1;
    plat->def_i2c_bus = bus;
    plat->i2c_bus[bus].bus_id = bus;
    plat->i2c_bus[bus].drv_type = drv_type;

    // i2c pins (these are virtual, entries are required to configure i2c layer)
    mraa_pincapabilities_t pinCapsI2c = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
    strncpy(plat->pins[pinIndex].name, "SDA", 8);
    plat->pins[pinIndex].capabilites = pinCapsI2c;
    plat->pins[pinIndex].i2c.mux_total = 0;
    plat->i2c_bus[bus].sda = pinIndex;
    pinIndex++;
    strncpy(plat->pins[pinIndex].name, "SCL", 8);
    plat->pins[pinIndex].capabilites = pinCapsI2c;
    plat->pins[pinIndex].i2c.mux_total = 0;
    plat->i2c_bus[bus].scl = pinIndex;

    // Set override functions
    mraa_drv_api_func_table[drv_type].i2c = mraa_i2c_ft4222_create_func_table();
    return plat;
}

