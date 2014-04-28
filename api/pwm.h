/*
 * Originally from mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 * Copyright (c) 2014 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

/** @file
 *
 * This file defines the pwm interface for libmaa
 *
 */

#include <stdio.h>
#include <fcntl.h>

#include "maa.h"

typedef struct {
    int chipid, pin;
    FILE *duty_fp;
} maa_pwm_context;

maa_pwm_context* maa_pwm_init(int chipin, int pin);

/** Set the ouput duty-cycle percentage, as a float
 *
 *  @param percentage A floating-point value representing percentage of output.
 *    The value should lie between 0.0f (representing on 0%) and 1.0f
 *    Values above or below this range will be set at either 0.0f or 1.0f.
 */
void maa_pwm_write(maa_pwm_context* pwm, float percentage);

/** Read the ouput duty-cycle percentage, as a float
 *
 *  @return percentage A floating-point value representing percentage of output.
 *    The value should lie between 0.0f (representing on 0%) and 1.0f
 *    Values above or below this range will be set at either 0.0f or 1.0f.
 */
float maa_pwm_read(maa_pwm_context* pwm);

/** Set the PWM period as seconds represented in a float
 *
 *  @param seconds Peroid represented as a float in seconds.
 */
void maa_pwm_period(maa_pwm_context* pwm, float seconds);

/** Set period. milli-oseconds.
 *  @param ms milli-seconds for period.
 */
void maa_pwm_period_ms(maa_pwm_context* pwm, int ms);

/** Set period. microseconds
 *  @param ns microseconds as period.
 */
void maa_pwm_period_us(maa_pwm_context* pwm, int us);

/** Set pulsewidth, As represnted by seconds in a (float).
 *  @param seconds The duration of a pulse
 */
void maa_pwm_pulsewidth(maa_pwm_context* pwm, float seconds);

 /** Set pulsewidth. Milliseconds
 *  @param ms milliseconds for pulsewidth.
 */
void maa_pwm_pulsewidth_ms(maa_pwm_context* pwm, int ms);

  /** Set pulsewidth, microseconds.
 *  @param us microseconds for pulsewidth.
 */
void maa_pwm_pulsewidth_us(maa_pwm_context* pwm, int us);

/** Set the enable status of the PWM pin. None zero will assume on with output being driven.
 *   and 0 will disable the output.
 *  @param enable enable status of pin
 */
void maa_pwm_enable(maa_pwm_context* pwm, int enable);

 /** Close and unexport the PWM pin.
 */
void maa_pwm_close(maa_pwm_context* pwm);
