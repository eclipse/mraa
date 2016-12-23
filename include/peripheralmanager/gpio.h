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

#ifndef SYSTEM_PERIPHERALMANAGER_GPIO_H_
#define SYSTEM_PERIPHERALMANAGER_GPIO_H_

#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

/// @defgroup Gpio Gpio Interface
/// @brief Functions to control GPIO pins.
///
/// These functions can be used to control GPIO.
/// @{

/// Edge trigger types.
enum {
  NONE_EDGE,  /**<  None */
  RISING_EDGE,  /**<  Rising edge */
  FALLING_EDGE,  /**<  Falling edge */
  BOTH_EDGE  /**<  Both edges */
};

/// GPIO direction types.
enum {
  DIRECTION_IN,  /**<  Input mode */
  DIRECTION_OUT_INITIALLY_HIGH,  /**<  Output mode, initially set to high */
  DIRECTION_OUT_INITIALLY_LOW  /**<  Output mode, initially set to low */
};

/// Possible active types.
enum {
  ACTIVE_LOW,  /**<  Active Low */
  ACTIVE_HIGH  /**<  Active High */
};

typedef struct BGpio BGpio;

/// Sets the GPIO direction to output.
/// @param gpio Pointer to the BGpio struct
/// @param direction One of DIRECTION_IN,
/// DIRECTION_OUT_INITIALLY_HIGH, DIRECTION_OUT_INITIALLY_LOW.
/// @return 0 on success, errno on error.
int BGpio_setDirection(const BGpio* gpio, int direction);

/// Sets the interrupt edge trigger type.
/// @param gpio Pointer to the BGpio struct
/// @param type One of NONE_EDGE, RISING_EDGE, FALLING_EDGE or BOTH_EDGE.
/// @return 0 on success, errno on error.
int BGpio_setEdgeTriggerType(const BGpio* gpio, int type);

/// Sets the GPIO’s active low/high status.
/// @param gpio Pointer to the BGpio struct.
/// @param type One of ACTIVE_HIGH, ACTIVE_LOW.
/// @return 0 on success, errno on error.
int BGpio_setActiveType(const BGpio* gpio, int type);

/// Sets the GPIO value (for output GPIO only).
/// @param gpio Pointer to the BGpio struct.
/// @param value Value to set.
/// @return 0 on success, errno on error.
int BGpio_setValue(const BGpio* gpio, int value);

/// Gets the GPIO value (for input GPIO only).
/// @param gpio Pointer to the BGpio struct.
/// @param value Output pointer to the value of the GPIO.
/// @return 0 on success, errno on error.
int BGpio_getValue(const BGpio* gpio, int* value);

/// Returns a file descriptor that can be used to poll on new data.
/// Can be passed to select/epoll to wait for data to become available.
/// @param gpio Pointer to the BGpio struct.
/// @param fd Output pointer to the file descriptor number.
/// @return 0 on success, errno on error.
int BGpio_getPollingFd(const BGpio* gpio, int* fd);

/// Acknowledges the interrupt and resets the file descriptor.
/// This must be called after each event triggers in order to be able to
/// poll/select for another event.
/// @param fd Polling file descriptor to reset.
/// @return 0 on success, errno on error.
int BGpio_ackInterruptEvent(int fd);

/// Destroys a BGpio struct.
/// @param gpio Pointer to the BGpio struct.
void BGpio_delete(BGpio* gpio);

/// @}

__END_DECLS

#endif  // SYSTEM_PERIPHERALMANAGER_GPIO_H_
