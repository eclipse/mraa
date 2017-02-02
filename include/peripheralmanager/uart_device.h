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

#ifndef SYSTEM_PERIPHERALMANAGER_UART_DEVICE_H_
#define SYSTEM_PERIPHERALMANAGER_UART_DEVICE_H_

#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

/// @defgroup Uart Uart device interface
/// @brief Functions to control an UART device.
///
/// These functions can be used to control an UART device.
/// @{

/// UART Parity
typedef enum AUartParity {
  AUART_PARITY_NONE = 0, /**< No parity */
  AUART_PARITY_EVEN = 1, /**< Even parity */
  AUART_PARITY_ODD = 2,  /**< Odd parity */
  AUART_PARITY_MARK = 3, /**< Mark parity, always 1 */
  AUART_PARITY_SPACE = 4 /**< Space parity, always 0 */
} AUartParity;

/// Modem control lines.
typedef enum AUartModemControlLine {
  AUART_MODEM_CONTROL_LE = 1 << 0,  /**< Data set ready/Line enable */
  AUART_MODEM_CONTROL_DTR = 1 << 1, /**< Data terminal ready */
  AUART_MODEM_CONTROL_RTS = 1 << 2, /**< Request to send */
  AUART_MODEM_CONTROL_ST = 1 << 3,  /**< Secondary TXD */
  AUART_MODEM_CONTROL_SR = 1 << 4,  /**< Secondary RXD */
  AUART_MODEM_CONTROL_CTS = 1 << 5, /**< Clear to send */
  AUART_MODEM_CONTROL_CD = 1 << 6,  /**< Data carrier detect */
  AUART_MODEM_CONTROL_RI = 1 << 7,  /**< Ring */
  AUART_MODEM_CONTROL_DSR = 1 << 8  /**< Data set ready */
} AUartModemControlLine;

// Hardware Flow Control
typedef enum AUartHardwareFlowControl {
  AUART_HARDWARE_FLOW_CONTROL_NONE = 0,       /**< No hardware flow control */
  AUART_HARDWARE_FLOW_CONTROL_AUTO_RTSCTS = 1 /**< Auto RTS/CTS */
} AUartHardwareFlowControl;

/// Flush queue selection
typedef enum AUartFlushDirection {
  AUART_FLUSH_IN = 0,    /**< Flushes data received but not read */
  AUART_FLUSH_OUT = 1,   /**< Flushes data written but not transmitted */
  AUART_FLUSH_IN_OUT = 2 /**< Flushes both in and out */
} AUartFlushDirection;

typedef struct AUartDevice AUartDevice;

/// Writes to a UART device.
/// @param device Pointer to the AUartDevice struct.
/// @param data Data to write.
/// @param len Size of the data to write.
/// @param bytes_written Output pointer to the number of bytes written.
/// @return 0 on success, errno on error.
int AUartDevice_write(const AUartDevice* device,
                      const void* data,
                      uint32_t len,
                      uint32_t* bytes_written);

/// Reads from a UART device.
/// @param device Pointer to the AUartDevice struct.
/// @param data Buffer to read the data into.
/// @param len Number of bytes to read.
/// @param bytes_read Output pointer to the number of bytes read.
/// @return 0 on success, errno on error.
int AUartDevice_read(const AUartDevice* device,
                     void* data,
                     uint32_t len,
                     uint32_t* bytes_read);

/// Sets the input and output speed of a UART device.
/// @param device Pointer to the AUartDevice struct.
/// @param baudrate Speed in baud.
/// @return 0 on success, errno on error.
int AUartDevice_setBaudrate(const AUartDevice* device, uint32_t baudrate);

/// Sets number of stop bits for the UART device.
/// @param device Pointer to the AUartDevice struct.
/// @param stop_bits Number of stop bits. Typically 1 or 2.
/// @return 0 on success, errno on error.
int AUartDevice_setStopBits(const AUartDevice* device, uint32_t stop_bits);

/// Sets the data size of a character for the UART device.
/// @param device Pointer to the AUartDevice struct.
/// @param data_size Number of bits per character. Typically between 5 and 8.
/// @return 0 on success, errno on error.
int AUartDevice_setDataSize(const AUartDevice* device, uint32_t data_size);

/// Sets the parity mode for the UART device.
/// @param device Pointer to the AUartDevice struct.
/// @param mode Parity mode.
/// @return 0 on success, errno on error.
int AUartDevice_setParity(const AUartDevice* device, AUartParity mode);

/// Sets the hardware flow control mode for the UART device.
/// @param device Pointer to the AUartDevice struct.
/// @param mode Flow control mode.
/// @return 0 on success, errno on error.
int AUartDevice_setHardwareFlowControl(const AUartDevice* device,
                                       AUartHardwareFlowControl mode);

/// Sets the modem control bits for the UART device.
/// @param device Pointer to the AUartDevice struct.
/// @param lines Lines to set. AUartModemControlLine values OR'ed together.
/// @return 0 on success, errno on error.
int AUartDevice_setModemControl(const AUartDevice* device, uint32_t lines);

/// Clears the modem control bits for the UART device.
/// @param device Pointer to the AUartDevice struct.
/// @param lines Lines to clear. AUartModemControlLine values OR'ed together.
/// @return 0 on success, errno on error.
int AUartDevice_clearModemControl(const AUartDevice* device, uint32_t lines);

/// Sends a break to the UART device.
/// @param device Pointer to the AUartDevice struct.
/// @param duration Duration of break transmission in milliseconds. If 0,
/// transmits zero-valued bits for at least 0.25 seconds, and not more
/// than 0.5 seconds.
/// @return 0 on success, errno on error.
int AUartDevice_sendBreak(const AUartDevice* device, uint32_t duration_msecs);

/// Flushes specified queue for the UART device.
/// @param device Pointer to the AUartDevice struct.
/// @param direction Direction to flush.
/// @return 0 on success, errno on error.
int AUartDevice_flush(const AUartDevice* device, AUartFlushDirection direction);

/// Gets a file descriptor to be notified when data can be read.
///
/// You can use this file descriptor to poll on incoming data instead of
/// actively reading for new data.
///
/// @param device Pointer to the AUartDevice struct.
/// @param fd Output pointer to the file descriptor.
/// @return 0 on success, errno on error.
int AUartDevice_getPollingFd(const AUartDevice* device, int* fd);

/// Acknowledges an input event.
///
/// This must be called after receiving an event notification on the polling
/// file descriptor.
/// If you don't acknowledge an event, peripheral manager will assume you are
/// still processing it and you will not receive any more events.
/// If you acknowledge an event before reading the data from the device, you
/// will receive an event immediately as there will still be data available.
///
/// @param fd File descriptor to acknowledge the event on.
/// @return 0 on success, errno on error.
int AUartDevice_ackInputEvent(int fd);

/// Destroys a AUartDevice struct.
/// @param device Pointer to the AUartDevice struct.
void AUartDevice_delete(AUartDevice* device);

/// @}

__END_DECLS

#endif  // SYSTEM_PERIPHERALMANAGER_UART_DEVICE_H_
