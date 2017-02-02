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

#ifndef SYSTEM_PERIPHERALMANAGER_SPI_DEVICE_H_
#define SYSTEM_PERIPHERALMANAGER_SPI_DEVICE_H_

#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

/// @defgroup Spi Spi device interface
/// @brief Functions to control an SPI device.
///
/// These functions can be used to control an SPI device.
/// @{

/// Endianness.
typedef enum ASpiBitJustification {
  ASPI_LSB_FIRST = 0, /**< Least significant bits first */
  ASPI_MSB_FIRST = 1  /**< Most significant bits first */
} ASpiBitJustification;

/// SPI modes (similar to the Linux kernel's modes).
typedef enum ASpiMode {
  ASPI_MODE0 = 0, /**< CPHA=0, CPOL=0 */
  ASPI_MODE1 = 1, /**< CPHA=1, CPOL=0 */
  ASPI_MODE2 = 2, /**< CPHA=0, CPOL=1 */
  ASPI_MODE3 = 3  /**< CPHA=1, CPOL=1 */
} ASpiMode;

typedef struct ASpiDevice ASpiDevice;

/// Writes a buffer to the device.
/// @param device Pointer to the ASpiDevice struct.
/// @param data Buffer to write.
/// @param len Length of the buffer.
/// @return 0 on success, errno on error.
int ASpiDevice_writeBuffer(const ASpiDevice* device,
                           const void* data,
                           size_t len);

/// Reads a buffer from the device.
/// @param device Pointer to the ASpiDevice struct.
/// @param data Buffer to read into.
/// @param len Length of the buffer.
/// @return 0 on success, errno on error.
int ASpiDevice_readBuffer(const ASpiDevice* device, void* data, size_t len);

/// Transfer data to the device.
/// @param device Pointer to the ASpiDevice struct.
/// @param tx_data Buffer to write.
/// @param rx_data Buffer to read data in. If NULL, no data will be read.
/// @param len Length of the buffers.
/// @return 0 on success, errno on error.
int ASpiDevice_transfer(const ASpiDevice* device,
                        const void* tx_data,
                        void* rx_data,
                        size_t len);

/// Sets the frequency in Hertz.
/// @param device Pointer to the ASpiDevice struct.
/// @param freq_hz Frequency to set.
/// @return 0 on success, errno on error.
int ASpiDevice_setFrequency(const ASpiDevice* device, uint32_t freq_hz);

/// Sets the SPI mode.
/// @param device Pointer to the ASpiDevice struct.
/// @param mode Mode to use. One of SPI_MODE0, SPI_MODE1, SPI_MODE2, SPI_MODE3.
/// @return 0 on success, errno on error.
int ASpiDevice_setMode(const ASpiDevice* device, ASpiMode mode);

/// Sets the bit justification.
/// @param device Pointer to the ASpiDevice struct.
/// @param bit_justification One of SPI_LSB_FIRST OR SPI_MSB_FIRST.
/// @return 0 on success, errno on error.
int ASpiDevice_setBitJustification(const ASpiDevice* device,
                                   ASpiBitJustification bit_justification);

/// Sets the number of bits per words.
/// @param device Pointer to the ASpiDevice struct.
/// @param bits_per_word Number of bits per word.
/// @return 0 on success, errno on error.
int ASpiDevice_setBitsPerWord(const ASpiDevice* device, uint8_t bits_per_word);

/// Sets the delay to wait after each transfer.
/// @param device Pointer to the ASpiDevice struct.
/// @param delay_usecs Delay in microseconds.
/// @return 0 on success, errno on error.
int ASpiDevice_setDelay(const ASpiDevice* device, uint16_t delay_usecs);

/// Sets the chip select behavior after each transfer.
/// @param device Pointer to the ASpiDevice struct.
/// @param change If set, cs will be active between transfers.
/// @return 0 on success, errno on error.
int ASpiDevice_setCsChange(const ASpiDevice* device, int change);

/// Destroys a ASpiDevice struct.
/// @param device Pointer to the ASpiDevice struct.
void ASpiDevice_delete(ASpiDevice* device);

/// @}

__END_DECLS

#endif  // SYSTEM_PERIPHERALMANAGER_SPI_DEVICE_H_
