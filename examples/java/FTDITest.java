/*
 * Author: Petre Eftime <petre.p.eftime@intel.com>
 * Copyright (c) 2016 Intel Corporation.
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

