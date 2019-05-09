/*
 * Author: Alexander Komarov <alexander.komarov@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import mraa.Dir;
import mraa.Edge;
import mraa.Gpio;

public class Isr {
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
        int pin = 6;
        if (argv.length == 1) {
            try {
                pin = Integer.parseInt(argv[0]);
            } catch (Exception e) {
            }
        }
        BufferedReader console = new BufferedReader(new InputStreamReader(System.in));
        Gpio gpio = null;
        try {
            gpio = new Gpio(pin);
        }  catch (Exception e) {
            System.out.println(e.getMessage());
            return;
        }
        System.out.println("Starting ISR for pin " + Integer.toString(pin) + ". Press ENTER to stop");
        Runnable callback = new JavaCallback(gpio);
        gpio.isr(Edge.EDGE_RISING, callback);
        try {
            String input = console.readLine();
        } catch (IOException e) {
        }
        gpio.isrExit();
    }

}

class JavaCallback implements Runnable {
    private Gpio gpio;

    public JavaCallback(Gpio gpio) {
        this.gpio = gpio;
    }

    @Override
    public void run() {
        String pin = Integer.toString(gpio.getPin(true));
        String level = Integer.toString(gpio.read());
        System.out.println("Pin " + pin + " = " + level);
    }
}
