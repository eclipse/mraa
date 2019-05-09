/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 * Author: Jakub Kramarz <jkramarz@virtuslab.com>
 * Copyright (c) 2015 VirtusLab
 *
 * SPDX-License-Identifier: MIT
 */

import mraa.Pwm;

public class CyclePwm3 {
    static {
        try {
            System.loadLibrary("mraajava");
        } catch (UnsatisfiedLinkError e) {
            System.err.println(
                    "Native code library failed to load. See the chapter on Dynamic Linking Problems in the SWIG Java documentation for help.\n" +
                            e);
            System.exit(1);
        }
    }
    public static void main(String argv[]) throws InterruptedException {
        //! [Interesting]
        Pwm pwm = new mraa.Pwm(3);
        pwm.period_us(200);
        pwm.enable(true);

        float value = 0;
        for (int i = 100; i > 0; --i) {
            value += 0.01;
            pwm.write(value);
            Thread.sleep(50);
            if (value >= 1) {
                value = 0;
            }
        }
        //! [Interesting]
    }
}
