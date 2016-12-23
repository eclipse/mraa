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
#include <termios.h>
#include <unistd.h>

__BEGIN_DECLS

/// @defgroup Uart Uart device interface
/// @brief Functions to control an UART device.
///
/// These functions can be used to control an UART device.
/// @{

/// UART Parity
enum UartParity {
  UART_PARITY_NONE, /**< No parity */
  UART_PARITY_EVEN, /**< Even parity */
  UART_PARITY_ODD,  /**< Odd parity */
  UART_PARITY_MARK, /**< Mark parity, always 1 */
  UART_PARITY_SPACE /**< Space parity, always 0 */
};

/// Modem control Bits
enum UartModemControlBits {
  UART_MC_LE = TIOCM_LE,   /**< Data set ready/Line enable */
  UART_MC_DTR = TIOCM_DTR, /**< Data terminal ready */
  UART_MC_RTS = TIOCM_RTS, /**< Request to send */
  UART_MC_ST = TIOCM_ST,   /**< Secondary TXD */
  UART_MC_SR = TIOCM_SR,   /**< Secondary RXD */
  UART_MC_CTS = TIOCM_CTS, /**< Clear to send */
  UART_MC_CD = TIOCM_CAR,  /**< Data carrier detect */
  UART_MC_RI = TIOCM_RI,   /**< Ring */
  UART_MC_DSR = TIOCM_DSR  /**< Data set ready */
};

// Hardware Flow Control
enum UartHardwareFlowControlType {
  UART_HW_FLOW_NONE,       /**< No hardware flow control */
  UART_HW_FLOW_AUTO_RTSCTS /**< Auto RTS/CTS */
};

/// Flush queue selection
enum UartFlushQueueSelection {
  UART_FLUSH_IN = TCIFLUSH,     /**< Flushes data received but not read */
  UART_FLUSH_OUT = TCOFLUSH,    /**< Flushes data written but not transmitted */
  UART_FLUSH_IN_OUT = TCIOFLUSH /**< Flushes both in and out */
};

typedef struct BUartDevice BUartDevice;

/// Writes to a UART device.
/// @param device Pointer to the BUartDevice struct.
/// @param data Data to write.
/// @param len Size of the data to write.
/// @param bytes_written Output pointer to the number of bytes written.
/// @return 0 on success, errno on error.
int BUartDevice_write(const BUartDevice* device,
                      const void* data,
                      uint32_t len,
                      uint32_t* bytes_written);

/// Reads from a UART device.
/// @param device Pointer to the BUartDevice struct.
/// @param data Buffer to read the data into.
/// @param len Number of bytes to read.
/// @param bytes_read Output pointer to the number of bytes read.
/// @return 0 on success, errno on error.
int BUartDevice_read(const BUartDevice* device,
                     void* data,
                     uint32_t len,
                     uint32_t* bytes_read);

/// Sets the input and output speed of a UART device.
/// @param device Pointer to the BUartDevice struct.
/// @param baudrate Speed in baud.
/// @return 0 on success, errno on error.
int BUartDevice_setBaudrate(const BUartDevice* device, uint32_t baudrate);

/// Sets number of stop bits for the UART device.
/// @param device Pointer to the BUartDevice struct.
/// @param stop_bits Number of stop bits. Typically 1 or 2.
/// @return 0 on success, errno on error.
int BUartDevice_setStopBits(const BUartDevice* device, uint32_t stop_bits);

/// Sets the data size of a character for the UART device.
/// @param device Pointer to the BUartDevice struct.
/// @param data_size Number of bits per character. Typically between 5 and 8.
/// @return 0 on success, errno on error.
int BUartDevice_setDataSize(const BUartDevice* device, uint32_t data_size);

/// Sets the parity mode for the UART device.
/// @param device Pointer to the BUartDevice struct.
/// @param mode Parity mode. One of UART_PARITY_NONE, UART_PARITY_EVEN,
/// UART_PARITY_ODD, UART_PARITY_MARK, UART_PARITY_SPACE.
/// @return 0 on success, errno on error.
int BUartDevice_setParity(const BUartDevice* device, uint32_t mode);

/// Sets the hardware flow control mode for the UART device.
/// @param device Pointer to the BUartDevice struct.
/// @param mode Flow control mode. Either UART_HW_FLOW_NONE or
/// UART_HW_FLOW_AUTO_RTSCTS.
/// @return 0 on success, errno on error.
int BUartDevice_setHardwareFlowControl(const BUartDevice* device,
                                       uint32_t mode);

/// Sets the modem control bits for the UART device.
/// @param device Pointer to the BUartDevice struct.
/// @param bits Modem control bits to set.
/// @return 0 on success, errno on error.
int BUartDevice_setModemControl(const BUartDevice* device, uint32_t bits);

/// Clears the modem control bits for the UART device.
/// @param device Pointer to the BUartDevice struct.
/// @param bits Modem control bits to clear.
/// @return 0 on success, errno on error.
int BUartDevice_clearModemControl(const BUartDevice* device, uint32_t bits);

/// Sends a break to the UART device.
/// @param device Pointer to the BUartDevice struct.
/// @param duration Duration of break transmission in milliseconds. If 0,
/// transmits zero-valued bits for at least 0.25 seconds, and not more
/// than 0.5 seconds.
/// @return 0 on success, errno on error.
int BUartDevice_sendBreak(const BUartDevice* device, uint32_t duration_msecs);

/// Flushes specified queue for the UART device.
/// @param device Pointer to the BUartDevice struct.
/// @param queue Queue to flush. One of UART_FLUSH_IN, UART_FLUSH_OUT,
/// UART_FLUSH_IN_OUT.
/// @return 0 on success, errno on error.
int BUartDevice_flush(const BUartDevice* device, uint32_t queue);

/// Gets a file descriptor to be notified when data can be read.
///
/// You can use this file descriptor to poll on incoming data instead of
/// actively reading for new data.
///
/// @param device Pointer to the BUartDevice struct.
/// @param fd Output pointer to the file descriptor.
/// @return 0 on success, errno on error.
int BUartDevice_getPollingFd(const BUartDevice* device, int* fd);

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
int BUartDevice_ackInputEvent(int fd);

/// Destroys a BUartDevice struct.
/// @param device Pointer to the BUartDevice struct.
void BUartDevice_delete(BUartDevice* device);

/// @}

__END_DECLS

#endif  // SYSTEM_PERIPHERALMANAGER_UART_DEVICE_H_
