/*
 * Copyright (C) 2016 The Android Open Source Project
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

#ifndef SYSTEM_PERIPHERALMANAGER_I2C_DEVICE_H_
#define SYSTEM_PERIPHERALMANAGER_I2C_DEVICE_H_

#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

/// @defgroup I2c I2c device interface
/// @brief Functions to control an I2C device.
///
/// These functions can be used to control an I2C device.
/// @{

typedef struct AI2cDevice AI2cDevice;

/// Reads from the device.
/// @param device Pointer to the AI2cDevice struct.
/// @param data Output buffer to write the data to.
/// @param len Number of bytes to read.
/// @return 0 on success, errno on error
int AI2cDevice_read(const AI2cDevice* device, void* data, uint32_t len);

/// Reads a byte from an I2C register.
/// @param device Pointer to the AI2cDevice struct.
/// @param reg Register to read from.
/// @param val Output pointer to value to read.
/// @return 0 on success, errno on error
int AI2cDevice_readRegByte(const AI2cDevice* device, uint8_t reg, uint8_t* val);

/// Reads a word from an I2C register.
/// @param device Pointer to the AI2cDevice struct.
/// @param reg Register to read from.
/// @param val Output pointer to value to read.
/// @return 0 on success, errno on error
int AI2cDevice_readRegWord(const AI2cDevice* device,
                           uint8_t reg,
                           uint16_t* val);

/// Reads from an I2C register.
/// @param device Pointer to the AI2cDevice struct.
/// @param reg Register to read from.
/// @param data Output buffer to write the data to.
/// @param len Number of bytes to read.
/// @return 0 on success, errno on error
int AI2cDevice_readRegBuffer(const AI2cDevice* device,
                             uint8_t reg,
                             void* data,
                             uint32_t len);

/// Writes to the device.
/// @param device Pointer to the AI2cDevice struct.
/// @param data Buffer to write.
/// @param len Number of bytes to write.
/// @return 0 on success, errno on error
int AI2cDevice_write(const AI2cDevice* device, const void* data, uint32_t len);

/// Writes a byte to an I2C register.
/// @param device Pointer to the AI2cDevice struct.
/// @param reg Register to write to.
/// @param val Value to write.
/// @return 0 on success, errno on error
int AI2cDevice_writeRegByte(const AI2cDevice* device, uint8_t reg, uint8_t val);

/// Writes a word to an I2C register.
/// @param device Pointer to the AI2cDevice struct.
/// @param reg Register to write to.
/// @param val Value to write.
/// @return 0 on success, errno on error
int AI2cDevice_writeRegWord(const AI2cDevice* device,
                            uint8_t reg,
                            uint16_t val);

/// Writes to an I2C register.
/// @param device Pointer to the AI2cDevice struct.
/// @param reg Register to write to.
/// @param data Data to write.
/// @param len Number of bytes to write.
/// @return 0 on success, errno on error
int AI2cDevice_writeRegBuffer(const AI2cDevice* device,
                              uint8_t reg,
                              const void* data,
                              uint32_t len);

/// Destroys a AI2cDevice struct.
/// @param device Pointer to the AI2cDevice struct.
void AI2cDevice_delete(AI2cDevice* device);

/// @}

__END_DECLS

#endif  // SYSTEM_PERIPHERALMANAGER_I2C_DEVICE_H_
