/*
 * Author: Shiran Ben-Melech <shiran.ben-melech@intel.com>
 * Copyright (c) 2016 Intel Corporation.
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
#include <errno.h>
#include "firmata.h"
#include "mraa_internal.h"
#include "firmata/firmata.h"
#include "firmata/firmata_ble.h"

#define PLATFORM_NAME "FIRMATA_BLE"

static mraa_boolean_t liblittleb_load_success = true;

void *
mraa_firmata_ble_dlsym(const char *symbol)
{
    void *func = dlsym(liblittleb_lib, symbol);
    if (func == NULL) {
        syslog(LOG_ERR, "%s", dlerror());
        liblittleb_load_success = false;
    }
    return func;
}


mraa_result_t
mraa_firmata_ble_init()
{
    mraa_result_t mraaStatus = MRAA_ERROR_NO_RESOURCES;
    
    liblittleb_lib = dlopen("liblittleb.so", RTLD_LAZY);
    if (!liblittleb_lib) {
        syslog(LOG_WARNING, "liblittleb.so not found, skipping");
        return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
    }

    dl_lb_init = mraa_firmata_ble_dlsym("lb_init");
    dl_lb_destroy = mraa_firmata_ble_dlsym("lb_destroy");
    dl_lb_context_new = mraa_firmata_ble_dlsym("lb_context_new");
    dl_lb_context_free = mraa_firmata_ble_dlsym("lb_context_free");
    dl_lb_get_bl_devices = mraa_firmata_ble_dlsym("lb_get_bl_devices");
    dl_lb_connect_device = mraa_firmata_ble_dlsym("lb_connect_device");
    dl_lb_disconnect_device = mraa_firmata_ble_dlsym("lb_disconnect_device");
    dl_lb_pair_device = mraa_firmata_ble_dlsym("lb_pair_device");
    dl_lb_unpair_device = mraa_firmata_ble_dlsym("lb_unpair_device");
    dl_lb_get_ble_characteristic_by_characteristic_path = mraa_firmata_ble_dlsym("lb_get_ble_characteristic_by_characteristic_path");
    dl_lb_get_ble_characteristic_by_uuid = mraa_firmata_ble_dlsym("lb_get_ble_characteristic_by_uuid");
    dl_lb_get_ble_service_by_service_path = mraa_firmata_ble_dlsym("lb_get_ble_service_by_service_path");
    dl_lb_get_ble_service_by_uuid = mraa_firmata_ble_dlsym("lb_get_ble_service_by_uuid");
    dl_lb_get_ble_device_services = mraa_firmata_ble_dlsym("lb_get_ble_device_services");
    dl_lb_get_device_by_device_path = mraa_firmata_ble_dlsym("lb_get_device_by_device_path");
    dl_lb_get_device_by_device_name = mraa_firmata_ble_dlsym("lb_get_device_by_device_name");
    dl_lb_get_device_by_device_address = mraa_firmata_ble_dlsym("lb_get_device_by_device_address");
    dl_lb_write_to_characteristic = mraa_firmata_ble_dlsym("lb_write_to_characteristic");
    dl_lb_read_from_characteristic = mraa_firmata_ble_dlsym("lb_read_from_characteristic");
    dl_lb_register_characteristic_read_event = mraa_firmata_ble_dlsym("lb_register_characteristic_read_event");
    dl_lb_parse_uart_service_message = mraa_firmata_ble_dlsym("lb_parse_uart_service_message");

    if (!liblittleb_load_success) {
        syslog(LOG_ERR, "Failed to find all symbols for littleb support");
        goto init_exit;
    }

    mraaStatus = MRAA_SUCCESS;

init_exit:
    if (mraaStatus == MRAA_SUCCESS)
        syslog(LOG_NOTICE, "mraa_firmata_ble_init completed successfully\n");
    return mraaStatus;
}