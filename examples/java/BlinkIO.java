/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 * Author: Petre Eftime <petre.p.eftime@intel.com>
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */


import mraa.Dir;
import mraa.Gpio;
import mraa.Result;
import mraa.mraa;

public class BlinkIO {
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

    final static int DEFAULT_IOPIN = 8;

    public static void main(String argv[]) throws InterruptedException {
        int iopin = DEFAULT_IOPIN;
        if (argv.length == 0) {
            System.out.println("Provide an int arg if you want to flash on something other than " + DEFAULT_IOPIN);
        } else {
            iopin = Integer.valueOf(argv[0]);
        }

        //! [Interesting]
        Gpio gpio = new Gpio(iopin);
        Result result = gpio.dir(Dir.DIR_OUT);
        if (result != Result.SUCCESS) {
            mraa.printError(result);
            System.exit(1);
        }

        for (int i = 100; i > 0; --i) {
            gpio.write(1);
            Thread.sleep(1000);
            gpio.write(0);
            Thread.sleep(1000);
        }
        //! [Interesting]
    }
}