/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 * Author: Jakub Kramarz <jkramarz@virtuslab.com>
 * Copyright (c) 2015 VirtusLab
 *
 * SPDX-License-Identifier: MIT
 */

import mraa.Dir;
import mraa.Gpio;
import mraa.mraa;

public class GpioRead6 {
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
        System.out.println(String.format("MRAA Version: %s\nStarting Read on IO6\n", mraa.getVersion()));
        //! [Interesting]
        Gpio gpio = new Gpio(6);

        gpio.dir(Dir.DIR_IN);

        for (int i = 100; i > 0; --i) {
            System.out.format("Gpio is %d\n", gpio.read());
            Thread.sleep(1000);
        }
        //! [Interesting]
    }
}
