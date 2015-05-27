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

mraa_result_t
mraa_ftdi_ft4222_get_version(unsigned int* versionChip, unsigned int* versionLib)
{
    int                  retCode;
    mraa_result_t        mraaStatus = MRAA_SUCCESS;
    FT_STATUS            ftStatus;
    FT_HANDLE            ftHandle = (FT_HANDLE)NULL;
    FT4222_STATUS        ft4222Status;
    FT4222_Version       ft4222Version;

    // Assume FT4222 is first FTDI device found.
    ftStatus = FT_Open(0, &ftHandle);
    if (ftStatus != FT_OK)
    {
        syslog(LOG_ERR, "libmraa: FT_Open failed (error %d)\n", (int)ftStatus);
        mraaStatus = MRAA_ERROR_NO_RESOURCES;
        goto exit;
    }

    // Get version of library and chip.    
    ft4222Status = FT4222_GetVersion(ftHandle, &ft4222Version);
    if (FT4222_OK != ft4222Status)
    {
        syslog(LOG_ERR, "libmraa: FT4222_GetVersion failed (error %d)\n", (int)ft4222Status);
        mraaStatus = MRAA_ERROR_NO_RESOURCES;
        goto exit;
    }

/*
    printf("Chip version: %08X, LibFT4222 version: %08X\n",
           (unsigned int)ft4222Version.chipVersion,
           (unsigned int)ft4222Version.dllVersion);
*/
    
exit:
    if (ftHandle != (FT_HANDLE)NULL)
    {
        (void)FT_Close(ftHandle);
    }

    return mraaStatus;
}


mraa_board_t*
mraa_ftdi_ft4222()
{
    mraa_board_t* b = (mraa_board_t*) malloc(sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }

    int i;
    int numPins = 4;
    b->platform_name = PLATFORM_NAME;
    b->phy_pin_count = numPins;
    b->gpio_count = numPins; 
    b->aio_count = 0;
    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * numPins);
    if (b->pins == NULL) {
        return NULL;
    }

    for (i=0; i<numPins; ++i) {
        char name[8];
        sprintf(name, "Pin%d", i);
        strncpy(b->pins[i].name, name, 8);
        b->pins[i].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }

    return b;
}

