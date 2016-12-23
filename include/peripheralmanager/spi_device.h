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
enum {
  SPI_LSB_FIRST,  /**< Least significant bits first */
  SPI_MSB_FIRST  /**< Most significant bits first */
};

/// SPI phase modes.
enum {
  SPI_CPHA = 0x01,  /**< Clock phase */
  SPI_CPOL = 0x02  /**< Clock polarity */
};

/// SPI modes (similar to the Linux kernel's modes).
enum {
  SPI_MODE0 = 0,  /**< CPHA=0, CPOL=0 */
  SPI_MODE1 = SPI_CPHA,  /**< CPHA=1, CPOL=0 */
  SPI_MODE2 = SPI_CPOL,  /**< CPHA=0, CPOL=1 */
  SPI_MODE3 = SPI_CPHA + SPI_CPOL  /**< CPHA=1, CPOL=1 */
};

typedef struct BSpiDevice BSpiDevice;

/// Writes a buffer to the device.
/// @param device Pointer to the BSpiDevice struct.
/// @param data Buffer to write.
/// @param len Length of the buffer.
/// @return 0 on success, errno on error.
int BSpiDevice_writeBuffer(const BSpiDevice* device,
                           const void* data,
                           size_t len);

/// Reads a buffer from the device.
/// @param device Pointer to the BSpiDevice struct.
/// @param data Buffer to read into.
/// @param len Length of the buffer.
/// @return 0 on success, errno on error.
int BSpiDevice_readBuffer(const BSpiDevice* device,
                          void* data,
                          size_t len);

/// Transfer data to the device.
/// @param device Pointer to the BSpiDevice struct.
/// @param tx_data Buffer to write.
/// @param rx_data Buffer to read data in. If NULL, no data will be read.
/// @param len Length of the buffers.
/// @return 0 on success, errno on error.
int BSpiDevice_transfer(const BSpiDevice* device,
                        const void* tx_data,
                        void* rx_data,
                        size_t len);

/// Sets the frequency in Hertz.
/// @param device Pointer to the BSpiDevice struct.
/// @param freq_hz Frequency to set.
/// @return 0 on success, errno on error.
int BSpiDevice_setFrequency(const BSpiDevice* device, uint32_t freq_hz);

/// Sets the SPI mode.
/// @param device Pointer to the BSpiDevice struct.
/// @param mode Mode to use. One of SPI_MODE0, SPI_MODE1, SPI_MODE2, SPI_MODE3.
/// @return 0 on success, errno on error.
int BSpiDevice_setMode(const BSpiDevice* device, int mode);

/// Sets the bit justification.
/// @param device Pointer to the BSpiDevice struct.
/// @param bit_justification One of SPI_LSB_FIRST OR SPI_MSB_FIRST.
/// @return 0 on success, errno on error.
int BSpiDevice_setBitJustification(const BSpiDevice* device,
                                   int bit_justification);

/// Sets the number of bits per words.
/// @param device Pointer to the BSpiDevice struct.
/// @param bits_per_word Number of bits per word.
/// @return 0 on success, errno on error.
int BSpiDevice_setBitsPerWord(const BSpiDevice* device, uint8_t bits_per_word);

/// Sets the delay to wait after each transfer.
/// @param device Pointer to the BSpiDevice struct.
/// @param delay_usecs Delay in microseconds.
/// @return 0 on success, errno on error.
int BSpiDevice_setDelay(const BSpiDevice* device, uint16_t delay_usecs);

/// Sets the chip select behavior after each transfer.
/// @param device Pointer to the BSpiDevice struct.
/// @param change If set, cs will be active between transfers.
/// @return 0 on success, errno on error.
int BSpiDevice_setCsChange(const BSpiDevice* device, int change);

/// Destroys a BSpiDevice struct.
/// @param device Pointer to the BSpiDevice struct.
void BSpiDevice_delete(BSpiDevice* device);

/// @}

__END_DECLS

#endif  // SYSTEM_PERIPHERALMANAGER_SPI_DEVICE_H_
