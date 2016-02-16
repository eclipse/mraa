/*
 * Copyright (c) 2016 Intel Corporation
 * Copyright (c) 2015 Jules Dourlens (jdourlens@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "firmata/servo.h"

#include <stdlib.h>
#include <stdio.h>

t_servo*
servo_attach(t_firmata* firmata, int pin)
{
    t_servo* res;

    if (!firmata || !firmata->isReady) {
        perror("servo_new::Firmata is not ready");
        return (NULL);
    }
    res = malloc(sizeof(t_servo));
    if (!res) {
        perror("servo_new::malloc failed");
        return (NULL);
    }
    res->firmata = firmata;
    res->pin = pin;
    firmata_pinMode(res->firmata, pin, MODE_SERVO);

    return res;
}

int
servo_write(t_servo* servo, int value)
{
    return (firmata_analogWrite(servo->firmata, servo->pin, value));
}
