import mraa.Dir;
import mraa.Gpio;
import mraa.Platform;
import mraa.mraa;

/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 * Author: Jakub Kramarz <jkramarz@virtuslab.com>
 * Copyright (c) 2015 VirtusLab
 *
 * SPDX-License-Identifier: MIT
 */

public class BlinkOnboard {
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
        Platform platform = mraa.getPlatformType();
        Gpio gpio, gpio_in;
        if (platform == Platform.INTEL_GALILEO_GEN1) {
            gpio = new Gpio(3);
        } else if (platform == Platform.INTEL_MINNOWBOARD_MAX) {
            gpio = new Gpio(21);
        } else {
            gpio = new Gpio(13);
        }
        System.out.format("Welcome to libmraa\n Version: %s\n Running on %s\n",
                mraa.getVersion(), platform.toString());

        gpio.dir(Dir.DIR_OUT);
        // on platforms with physical button use gpio_in
        if (platform == Platform.INTEL_MINNOWBOARD_MAX) {
            gpio_in = new Gpio(14);
            gpio_in.dir(Dir.DIR_IN);
            System.out.println("Press and hold S1 to stop, Press SW1 to shutdown!");
        } else {
            gpio_in = null;
        }

        boolean state = false;
        for (int i = 100; i > 0; --i) {
            if (gpio_in != null && gpio_in.read() == 0) {
                return;
            }
            if (state) {
                state = false;
                gpio.write(1);
            } else {
                state = true;
                gpio.write(0);
            }
            Thread.sleep(1000);
        }
    }
}