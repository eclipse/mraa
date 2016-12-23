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

#ifndef SYSTEM_PERIPHERALMANAGER_PWM_H_
#define SYSTEM_PERIPHERALMANAGER_PWM_H_

#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

/// @defgroup Pwm Pwm Interface
/// @brief Functions to control PWM pins.
///
/// These functions can be used to control PWM.
/// @{

typedef struct BPwm BPwm;

/// Sets the PWM duty cycle.
/// @param gpio Pointer to the BPwm struct
/// @param duty_cycle Double between 0 and 100 inclusive.
/// @return 0 on success, errno on error.
int BPwm_setDutyCycle(const BPwm* pwm, double duty_cycle);

/// Sets the PWM frequency.
/// @param gpio Pointer to the BPwm struct
/// @param freq Double denoting the frequency in Hz.
/// @return 0 on success, errno on error.
int BPwm_setFrequencyHz(const BPwm* pwm, double frequency);

/// Enables the PWM.
/// @param gpio Pointer to the BPwm struct
/// @return 0 on success, errno on error.
int BPwm_enable(const BPwm* pwm);

/// Disables the PWM.
/// @param gpio Pointer to the BPwm struct
/// @return 0 on success, errno on error.
int BPwm_disable(const BPwm* pwm);

/// Destroys a BPwm struct.
/// @param pwm Pointer to the BPwm struct.
void BPwm_delete(BPwm* pwm);

/// @}

__END_DECLS

#endif  // SYSTEM_PERIPHERALMANAGER_PWM_H_
