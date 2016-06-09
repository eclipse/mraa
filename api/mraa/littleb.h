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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <syslog.h>
#include <pthread.h>
#include <systemd/sd-bus.h>

#define MAX_LEN 256
#define MAX_OBJECTS 256

static const char* BLUEZ_DEST = "org.bluez";
static const char* BLUEZ_DEVICE = "org.bluez.Device1";
static const char* BLUEZ_GATT_SERVICE = "org.bluez.GattService1";
static const char* BLUEZ_GATT_CHARACTERISTICS = "org.bluez.GattCharacteristic1";

/**
 * LB return codes
 */
typedef enum {
    LB_SUCCESS = 0,                       /**< Expected response */
    LB_ERROR_FEATURE_NOT_IMPLEMENTED = 1, /**< Feature TODO */
    LB_ERROR_FEATURE_NOT_SUPPORTED = 2,   /**< Feature not supported by HW */
    LB_ERROR_INVALID_DEVICE = 3,          /**< Not a BL or BLE device */
    LB_ERROR_INVALID_BUS = 4,             /**< sd bus invalid */
    LB_ERROR_NO_RESOURCES = 5,            /**< No resource of that type avail */
    LB_ERROR_MEMEORY_ALLOCATION = 6,      /**< Memory allocation fail */
    LB_ERROR_SD_BUS_CALL_FAIL = 7,        /**< sd_bus call failure */

    LB_ERROR_UNSPECIFIED = 99 /**< Unknown Error */
} lb_result_t;

typedef struct ble_characteristic {
    const char* char_path; /**< device path under dbus */
    const char* uuid;      /**< uuid of the characteristic. */
} ble_char;

typedef struct ble_service {
    const char* service_path;   /**< device path under dbus */
    const char* uuid;           /**< uuid of the service. */
    bool primary;               /**< is the service primary in the device */
    ble_char** characteristics; /**< list of the characteristics inside the service */
    int characteristics_size;   /**< count of characteristics in the service */
} ble_service;

typedef struct bl_device {
    const char* device_path; /**< device path under dbus */
    const char* address;     /**< address of the bluetooth device */
    const char* name;        /**< name of the bluetooth device */
    ble_service** services;  /**< list of the service inside the device */
    int services_size;       /**< count of services in the device */
} bl_device;


typedef struct lb_context {
    sd_bus* bus;         /**< system bus to be used */
    bl_device** devices; /**< list of the devices found in a scan */
    int devices_size;    /**< count of devices found*/
} lb_context;

typedef struct event_callback_pair {
    const char* event;
    sd_bus_message_handler_t* callback;
} event_pair;


/**
 * Initialize littleb.
 *
 * Set event and event loop configuration
 *
 * @return Result of operation
 */
lb_result_t lb_init();

/**
 * Destroy littleb
 *
 * @return Result of operation
 */
lb_result_t lb_destroy();

/**
 * Create new littleb context to contain devices found
 *
 * @param ADD
 * @return new lb_context or NULL if failed
 */
lb_context* lb_context_new();

/**
 * Free littleb context created with lb_context_new
 *
 * @param lb_context to be freed
 * @return Result of operation
 */
lb_result_t lb_context_free(lb_context* lb_ctx);

/**
 * Populate internal list of bl devices found in a scan of specified length
 *
 * @param lb_context to use
 * @param seconds to perform device scan
 * @return Result of operation
 */
lb_result_t lb_get_bl_devices(lb_context* lb_ctx, int seconds);

/**
 * Connect to a specific bluetooth device
 *
 * bl_device can be found by name, path or address using lb_get_device functions
 *
 * @param lb_context to use
 * @param bl_device to connect to
 * @return Result of operation
 */
lb_result_t lb_connect_device(lb_context* lb_ctx, bl_device* dev);

/**
 * Disconnect from a specific bluetooth device
 *
 * @param lb_context to use
 * @param bl_device to disconnect from
 * @return Result of operation
 */
lb_result_t lb_disconnect_device(lb_context* lb_ctx, bl_device* dev);

/**
 * Pair with specific bluetooth device
 *
 * @param lb_context to use
 * @param bl_device to pair with
 * @return Result of operation
 */
lb_result_t lb_pair_device(lb_context* lb_ctx, bl_device* dev);

/**
 * Cancel pairing with specific bluetooth device
 *
 * @param lb_context to use
 * @param bl_device to cancel pair with
 * @return Result of operation
 */
lb_result_t lb_unpair_device(lb_context* lb_ctx, bl_device* dev);

/**
 * Populate ble_char with characteristic found by using it's device path under dbus
 *
 * @param lb_context to use
 * @param bl_dev to search the characteristic in
 * @param characteristic_path to search for
 * @param ble_char to populate with characteristic found
 * @return Result of operation
 */
lb_result_t lb_get_ble_characteristic_by_characteristic_path(lb_context* lb_ctx,
                                                             bl_device* dev,
                                                             const char* characteristic_path,
                                                             ble_char** ble_characteristic_ret);

/**
 * Populate ble_char with characteristic found by using it's uuid
 *
 * @param lb_context to use
 * @param bl_dev to search the characteristic in
 * @param uuid to search for
 * @param ble_char to populate with characteristic found
 * @return Result of operation
 */
lb_result_t
lb_get_ble_characteristic_by_uuid(lb_context* lb_ctx, bl_device* dev, const char* uuid, ble_char** ble_characteristic_ret);

/**
 * Populate ble_service with service found by using it's device path under dbus
 *
 * @param lb_context to use
 * @param bl_dev to search the service in
 * @param service_path to search for
 * @param ble_service to populate with service found
 * @return Result of operation
 */
lb_result_t lb_get_ble_service_by_service_path(lb_context* lb_ctx,
                                               bl_device* dev,
                                               const char* service_path,
                                               ble_service** ble_service_ret);

/**
 * Populate ble_service with service found by using it's uuid
 *
 * @param lb_context to use
 * @param bl_dev to search the service in
 * @param uuid to search for
 * @param ble_service to populate with service found
 * @return Result of operation
 */
lb_result_t lb_get_ble_service_by_uuid(lb_context* lb_ctx, bl_device* dev, const char* uuid, ble_service** ble_service_ret);

/**
 * Populate the BLE device with it's services
 *
 * @param lb_context to use
 * @param bl_dev to scan services
 * @return Result of operation
 */
lb_result_t lb_get_ble_device_services(lb_context* lb_ctx, bl_device* dev);

/**
 * Get bluetooth device by using it's device path under dbus
 *
 * @param lb_context to use
 * @param device_path to search for
 * @param bl_device_ret to populate with the found device
 * @return Result of operation
 */
lb_result_t lb_get_device_by_device_path(lb_context* lb_ctx, const char* device_path, bl_device** bl_device_ret);

/**
 * Get bluetooth device by searching for specific name
 *
 * Will return the first found device with the name specified
 *
 * @param lb_context to use
 * @param name to search for
 * @param bl_device_ret to populate with the found device
 * @return Result of operation
 */
lb_result_t lb_get_device_by_device_name(lb_context* lb_ctx, const char* name, bl_device** bl_device_ret);

/**
 * Get bluetooth device by searching for specific address
 *
 * @param lb_context to use
 * @param address to search for
 * @param bl_device_ret to populate with the found device
 * @return Result of operation
 */
lb_result_t lb_get_device_by_device_address(lb_context* lb_ctx, const char* address, bl_device** bl_device_ret);

/**
 * Write to a specific BLE device characteristic using it's uuid
 *
 * @param lb_context to use
 * @param dev BLE device to search the characteristic in
 * @param uuid of the characteristic to write to
 * @param size of the uint8 array to be written
 * @param the array of byte buffer to write to the characteristic
 * @return Result of operation
 */
lb_result_t
lb_write_to_characteristic(lb_context* lb_ctx, bl_device* dev, const char* uuid, int size, uint8_t* value);

/**
 * Read from a specific BLE device characteristic using it's uuid
 *
 * @param lb_context to use
 * @param dev BLE device to search the characteristic in
 * @param uuid of the characteristic to read from
 * @param size of the uint8 array that was read
 * @param the array of byte buffer that was read
 * @return Result of operation
 */
lb_result_t
lb_read_from_characteristic(lb_context* lb_ctx, bl_device* dev, const char* uuid, size_t* size, uint8_t** result);

/**
 * Register a callback function for an event of characteristic value change
 *
 * @param lb_context to use
 * @param dev BLE device to search the characteristic in
 * @param uuid of the characteristic to read from
 * @param callback function to be called when char value changed
 * @param userdata to pass in the callback function
 * @return Result of operation
 */
lb_result_t lb_register_characteristic_read_event(lb_context* lb_ctx,
                                                  bl_device* dev,
                                                  const char* uuid,
                                                  sd_bus_message_handler_t callback,
                                                  void* userdata);

/**
 * Special function to parse uart tx line buffer
 *
 * @param message sd_bus_message to prase the buffer array from
 * @param result buffer to accommodate the result
 * @param size of the buffer
 * @return Result of operation
 */
lb_result_t lb_parse_uart_service_message(sd_bus_message* message, const void** result, size_t* size);
#ifdef __cplusplus
}
#endif
