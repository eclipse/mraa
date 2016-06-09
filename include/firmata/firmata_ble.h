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
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <dlfcn.h>
#include "mraa_internal.h"
#include "littleb.h"

lb_result_t (*dl_lb_init)();
lb_result_t (*dl_lb_destroy)();
lb_context* (*dl_lb_context_new)();
lb_result_t (*dl_lb_context_free)(lb_context*);
lb_result_t (*dl_lb_get_bl_devices)(lb_context*, int);
lb_result_t (*dl_lb_connect_device)(lb_context*, bl_device*);
lb_result_t (*dl_lb_disconnect_device)(lb_context*, bl_device*);
lb_result_t (*dl_lb_pair_device)(lb_context*, bl_device*);
lb_result_t (*dl_lb_unpair_device)(lb_context*, bl_device*);
lb_result_t (*dl_lb_get_ble_characteristic_by_characteristic_path)(lb_context*, bl_device*, const char*, ble_char**);
lb_result_t (*dl_lb_get_ble_characteristic_by_uuid)(lb_context*, bl_device*, const char*, ble_char**);
lb_result_t (*dl_lb_get_ble_service_by_service_path)(lb_context* lb_ctx, bl_device*, const char*, ble_service**);
lb_result_t (*dl_lb_get_ble_service_by_uuid)(lb_context*, bl_device*, const char*, ble_service**);
lb_result_t (*dl_lb_get_ble_device_services)(lb_context*, bl_device*);
lb_result_t (*dl_lb_get_device_by_device_path)(lb_context*, const char*, bl_device**);
lb_result_t (*dl_lb_get_device_by_device_name)(lb_context*, const char*, bl_device**);
lb_result_t (*dl_lb_get_device_by_device_address)(lb_context*, const char*, bl_device**);
lb_result_t (*dl_lb_write_to_characteristic)(lb_context*, bl_device*, const char*, int, uint8_t*);
lb_result_t (*dl_lb_read_from_characteristic)(lb_context*, bl_device*, const char*, size_t*, uint8_t**);
lb_result_t (*dl_lb_register_characteristic_read_event)(lb_context*,bl_device*, const char*, sd_bus_message_handler_t, void*);
lb_result_t (*dl_lb_parse_uart_service_message)(sd_bus_message*, const void**, size_t*);

mraa_result_t mraa_firmata_ble_init();
void* liblittleb_lib;

#ifdef __cplusplus
}
#endif
