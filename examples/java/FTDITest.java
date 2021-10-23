/*
 * Author: Petre Eftime <petre.p.eftime@intel.com>
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

import mraa.*;

public class FTDITest {

    public static void main(String[] args) throws InterruptedException {
        String board = mraa.getPlatformName();
        String version = mraa.getVersion();

        System.out.println(String.format("Version: %s", version));
        System.out.println(String.format("Running on %s", board));

        if (mraa.hasSubPlatform()) {
            System.out.println("Subplatform detected");

            /* Print when button is pressed */
            Gpio button = new Gpio(515);
            button.dir(Dir.DIR_IN);
            button.isr(Edge.EDGE_FALLING, new Runnable() {

                @Override
                public void run() {
                    System.out.println("Button pressed");
                }
            });

            /* Blink FTDI board LED */
            Gpio led = new Gpio(514);
            led.dir(Dir.DIR_OUT);
            for (int i = 100; i > 0; --i) {
                led.write(i % 2);
                Thread.sleep(500);
            }
        } else {
            System.out.println("Subplatform not detected");
        }
    }
}

