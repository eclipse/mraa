/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SYSTEM_PERIPHERALMANAGER_PERIPHERAL_MANAGER_CLIENT_H_
#define SYSTEM_PERIPHERALMANAGER_PERIPHERAL_MANAGER_CLIENT_H_

#include <sys/cdefs.h>

#include "peripheralmanager/gpio.h"
#include "peripheralmanager/i2c_device.h"
#include "peripheralmanager/pwm.h"
#include "peripheralmanager/spi_device.h"
#include "peripheralmanager/uart_device.h"

__BEGIN_DECLS

/// @defgroup PeripheralManagerClient Peripheral client functions
/// @brief Functions to access embedded peripherals
/// @{

typedef struct APeripheralManagerClient APeripheralManagerClient;

/// Returns the list of GPIOs.
/// This does not take ownership into account.
/// The list must be freed by the caller.
/// @param client Pointer to the APeripheralManagerClient struct.
/// @param num_gpio Output pointer to the number of elements in the list.
/// @return The list of gpios.
char** APeripheralManagerClient_listGpio(const APeripheralManagerClient* client,
                                         int* num_gpio);

/// Opens a GPIO and takes ownership of it.
/// @param client Pointer to the APeripheralManagerClient struct.
/// @param name Name of the GPIO.
/// @param gpio Output pointer to the AGpio struct. Empty on error.
/// @return 0 on success, errno on error.
int APeripheralManagerClient_openGpio(const APeripheralManagerClient* client,
                                      const char* name,
                                      AGpio** gpio);

/// Returns the list of PWMs.
/// This does not take ownership into account.
/// The list must be freed by the caller.
/// @param client Pointer to the APeripheralManagerClient struct.
/// @param num_gpio Output pointer to the number of elements in the list.
/// @return The list of pwms.
char** APeripheralManagerClient_listPwm(const APeripheralManagerClient* client,
                                        int* num_pwm);

/// Opens a PWM and takes ownership of it.
/// @param client Pointer to the APeripheralManagerClient struct.
/// @param name Name of the PWM.
/// @param gpio Output pointer to the AGpio struct. Empty on error.
/// @return 0 on success, errno on error.
int APeripheralManagerClient_openPwm(const APeripheralManagerClient* client,
                                     const char* name,
                                     APwm** pwm);

/// Returns the list of SPI buses.
/// This does not take ownership into account.
/// The list must be freed by the caller.
/// @param client Pointer to the APeripheralManagerClient struct.
/// @param num_spi_buses Output pointer to the number of elements in the list.
/// @return The list of spi buses.
char** APeripheralManagerClient_listSpiBuses(
    const APeripheralManagerClient* client, int* num_spi_buses);

/// Opens a SPI device and takes ownership of it.
/// @oaram client Pointer to the APeripheralManagerClient struct.
/// @param name Name of the SPI device.
/// @param dev Output pointer to the ASpiDevice struct. Empty on error.
/// @return 0 on success, errno on error.
int APeripheralManagerClient_openSpiDevice(
    const APeripheralManagerClient* client, const char* name, ASpiDevice** dev);

/// Returns the list of I2C buses.
/// This does not take ownership into account.
/// The list must be freed by the caller.
/// @param client Pointer to the APeripheralManagerClient struct.
/// @param num_i2c_buses Output pointer to the number of elements in the list.
/// @return The list of i2c buses.
char** APeripheralManagerClient_listI2cBuses(
    const APeripheralManagerClient* client, int* num_i2c_buses);

/// Opens an I2C device and takes ownership of it.
/// @param client Pointer to the APeripheralManagerClient struct.
/// @param name Name of the I2C bus.
/// @param address Address of the I2C device.
/// @param dev Output pointer to the AI2cDevice struct. Empty on error.
/// @return 0 on success, errno on error
int APeripheralManagerClient_openI2cDevice(
    const APeripheralManagerClient* client,
    const char* name,
    uint32_t address,
    AI2cDevice** dev);

/// Returns the list of UART buses.
/// This does not take ownership into account.
/// The list must be freed by the caller.
/// @param client Pointer to the APeripheralManagerClient struct.
/// @param num_uart_buses Output pointer to the number of elements in the list.
/// @return The list of uart buses.
char** APeripheralManagerClient_listUartDevices(
    const APeripheralManagerClient* client, int* num_uart_buses);

/// Opens an UART device and takes ownership of it.
/// @param client Pointer to the APeripheralManagerClient struct.
/// @param name Name of the UART device.
/// @param dev Output pointer to the AUartDevice struct. Empty on error.
/// @return 0 on success, errno on error
int APeripheralManagerClient_openUartDevice(
    const APeripheralManagerClient* client,
    const char* name,
    AUartDevice** dev);

/// Creates a new client.
/// @return A pointer to the created client. nullptr on errors.
APeripheralManagerClient* APeripheralManagerClient_new();

/// Destroys the peripheral manager client.
/// @param client Pointer to the APeripheralManagerClient struct.
void APeripheralManagerClient_delete(APeripheralManagerClient* client);

/// @}

__END_DECLS

#endif  //  SYSTEM_PERIPHERALMANAGER_PERIPHERAL_MANAGER_CLIENT_H_
