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
typedef enum AGpioEdge {
  AGPIO_EDGE_NONE = 0,    /**<  None */
  AGPIO_EDGE_RISING = 1,  /**<  Rising edge */
  AGPIO_EDGE_FALLING = 2, /**<  Falling edge */
  AGPIO_EDGE_BOTH = 3     /**<  Both edges */
} AGpioEdge;

/// GPIO direction types.
typedef enum AGpioDirection {
  AGPIO_DIRECTION_IN = 0,                 /**<  Input mode */
  AGPIO_DIRECTION_OUT_INITIALLY_HIGH = 1, /**<  Output mode, initially high */
  AGPIO_DIRECTION_OUT_INITIALLY_LOW = 2   /**<  Output mode, initially low */
} AGpioDirection;

/// Possible active types.
typedef enum AGpioActiveType {
  AGPIO_ACTIVE_LOW = 0, /**<  Active Low */
  AGPIO_ACTIVE_HIGH = 1 /**<  Active High */
} AGpioActiveType;

typedef struct AGpio AGpio;

/// Sets the GPIO direction to output.
/// @param gpio Pointer to the AGpio struct
/// @param direction One of DIRECTION_IN,
/// DIRECTION_OUT_INITIALLY_HIGH, DIRECTION_OUT_INITIALLY_LOW.
/// @return 0 on success, errno on error.
int AGpio_setDirection(const AGpio* gpio, AGpioDirection direction);

/// Sets the interrupt edge trigger type.
/// @param gpio Pointer to the AGpio struct
/// @param type One of NONE_EDGE, RISING_EDGE, FALLING_EDGE or BOTH_EDGE.
/// @return 0 on success, errno on error.
int AGpio_setEdgeTriggerType(const AGpio* gpio, AGpioEdge type);

/// Sets the GPIO’s active low/high status.
/// @param gpio Pointer to the AGpio struct.
/// @param type One of ACTIVE_HIGH, ACTIVE_LOW.
/// @return 0 on success, errno on error.
int AGpio_setActiveType(const AGpio* gpio, AGpioActiveType type);

/// Sets the GPIO value (for output GPIO only).
/// @param gpio Pointer to the AGpio struct.
/// @param value Value to set.
/// @return 0 on success, errno on error.
int AGpio_setValue(const AGpio* gpio, int value);

/// Gets the GPIO value (for input GPIO only).
/// @param gpio Pointer to the AGpio struct.
/// @param value Output pointer to the value of the GPIO.
/// @return 0 on success, errno on error.
int AGpio_getValue(const AGpio* gpio, int* value);

/// Returns a file descriptor that can be used to poll on new data.
/// Can be passed to select/epoll to wait for data to become available.
/// @param gpio Pointer to the AGpio struct.
/// @param fd Output pointer to the file descriptor number.
/// @return 0 on success, errno on error.
int AGpio_getPollingFd(const AGpio* gpio, int* fd);

/// Acknowledges the interrupt and resets the file descriptor.
/// This must be called after each event triggers in order to be able to
/// poll/select for another event.
/// @param fd Polling file descriptor to reset.
/// @return 0 on success, errno on error.
int AGpio_ackInterruptEvent(int fd);

/// Destroys a AGpio struct.
/// @param gpio Pointer to the AGpio struct.
void AGpio_delete(AGpio* gpio);

/// @}

__END_DECLS

#endif  // SYSTEM_PERIPHERALMANAGER_GPIO_H_
