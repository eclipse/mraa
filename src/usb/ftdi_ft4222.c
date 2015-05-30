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
#include "common.h"
#include "ftd2xx.h"
#include "libft4222.h"
#include "usb/ftdi_ft4222.h"

#define PLATFORM_NAME "FTDI FT4222"

static FT_HANDLE ftHandle = (FT_HANDLE)NULL;


mraa_result_t
mraa_ftdi_ft4222_init(unsigned int* versionChip, unsigned int* versionLib)
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




static mraa_result_t 
mraa_ftdi_ft4222_i2c_init_bus_replace(mraa_i2c_context i2c_dev)
{
    // Tell the FT4222 to be an I2C Master.
    FT4222_STATUS ft4222Status = FT4222_I2CMaster_Init(ftHandle, 400);
    if (FT4222_OK != ft4222Status)
    {
        syslog(LOG_ERR, "FT4222_I2CMaster_Init failed (error %d)!\n", ft4222Status);
        return MRAA_ERROR_NO_RESOURCES;
    }
    
    // Reset the I2CM registers to a known state.
    ft4222Status = FT4222_I2CMaster_Reset(ftHandle);
    if (FT4222_OK != ft4222Status)
    {
        syslog(LOG_ERR, "FT4222_I2CMaster_Reset failed (error %d)!\n", ft4222Status);
        return MRAA_ERROR_NO_RESOURCES;
    }

    return MRAA_SUCCESS;
}


static mraa_result_t
mraa_ftdi_ft4222_i2c_address_replace(mraa_i2c_context dev, uint8_t addr)
{
    dev->addr = (int) addr;
    return MRAA_SUCCESS;
}


static int
mraa_ftdi_ft4222_i2c_read_replace(mraa_i2c_context dev, uint8_t* data, int length)
{
    uint16 bytesRead = 0;
    FT4222_STATUS ft4222Status = FT4222_I2CMaster_Read(ftHandle, dev->addr, data, length, &bytesRead);
    if (FT4222_OK != ft4222Status)
    {
        syslog(LOG_ERR, "FT4222_I2CMaster_Read failed (error %d)\n", (int)ft4222Status);
        return 0;
    }
    return bytesRead;
}



static int
mraa_ftdi_ft4222_i2c_write(mraa_i2c_context dev, uint8_t* data, int bytesToWrite)
{
    uint16 bytesWritten = 0;
    FT4222_STATUS ft4222Status = FT4222_I2CMaster_Write(ftHandle, dev->addr, data, bytesToWrite, &bytesWritten);
    if (FT4222_OK != ft4222Status)
    {
        syslog(LOG_ERR, "FT4222_I2CMaster_Write failed (error %d)\n", (int)ft4222Status);
        return 0;
    }

    if (bytesWritten != bytesToWrite)
        syslog(LOG_WARNING, "FT4222_I2CMaster_Write wrote %u of %u bytes.\n", bytesWritten, bytesToWrite);

   return (int)bytesWritten;
}


static mraa_result_t
mraa_ftdi_ft4222_i2c_write_byte_replace(mraa_i2c_context dev, uint8_t data)
{
    return mraa_ftdi_ft4222_i2c_write(dev, &data, 1) == 1 ? MRAA_SUCCESS : MRAA_ERROR_UNSPECIFIED;
}


mraa_board_t*
mraa_ftdi_ft4222()
{
    mraa_board_t* b = (mraa_board_t*) malloc(sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }

    int pinIndex = 0;
    int numGpio = 0;
    int numPins = numGpio + 2; // Add SDA and SCL
    b->platform_name = PLATFORM_NAME;
    b->phy_pin_count = numPins;
    b->gpio_count = numGpio; 
    b->aio_count = 0;
    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * numPins);
    if (b->pins == NULL) {
        return NULL;
    }

    // Virtual gpio pins on i2c I/O expander.
    mraa_pincapabilities_t pinCapsGpio = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    for (pinIndex=0; pinIndex < numGpio; ++pinIndex) {
        char name[8];
        sprintf(name, "Pin%d", pinIndex);
        strncpy(b->pins[pinIndex].name, name, 8);
        b->pins[pinIndex].capabilites = pinCapsGpio;
    }

    // i2c pins (these are virtual, entries are required to configure i2c layer)
    mraa_pincapabilities_t pinCapsI2c = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
    int bus = 0;
    b->i2c_bus[bus].scl = pinIndex;
    b->i2c_bus_count = 1;
    b->def_i2c_bus = bus;
    b->i2c_bus[bus].bus_id = bus;
    strncpy(b->pins[pinIndex].name, "SDA", 8);
    b->pins[pinIndex].capabilites = pinCapsI2c;
    b->pins[pinIndex].i2c.mux_total = 0;
    b->i2c_bus[bus].sda = pinIndex;
    pinIndex++;
    strncpy(b->pins[pinIndex].name, "SCL", 8);
    b->pins[pinIndex].capabilites = pinCapsI2c;
    b->pins[pinIndex].i2c.mux_total = 0;

    // Set override functions
    advance_func->i2c_init_bus_replace = &mraa_ftdi_ft4222_i2c_init_bus_replace;
    advance_func->i2c_address_replace = &mraa_ftdi_ft4222_i2c_address_replace;
    advance_func->i2c_read_replace = &mraa_ftdi_ft4222_i2c_read_replace;
    advance_func->i2c_write_byte_replace = &mraa_ftdi_ft4222_i2c_write_byte_replace;

    return b;
}

