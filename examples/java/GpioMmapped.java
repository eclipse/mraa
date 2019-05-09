/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 * Author: Petre Eftime <petre.p.eftime@intel.com>
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

import mraa.Gpio;
import mraa.mraa;

public class GpioMmapped {
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
    String board = mraa.getPlatformName();
    String version = mraa.getVersion();
    System.out.println("hello mraa");
    System.out.println(String.format("Version: %s", version));
    System.out.println(String.format("Platform: %s", board));
    
    Gpio gpio = new Gpio(1);
    
    gpio.useMmap(true);
    
    for (int i = 1000; i > 0; --i) {
        gpio.write(1);
        Thread.sleep(50);
        gpio.write(0);
        Thread.sleep(50);
    }
    //! [Interesting]
  };
}
