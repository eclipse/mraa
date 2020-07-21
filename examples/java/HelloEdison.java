/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Author: Petre Eftime <petre.p.eftime@intel.com>
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

//! [Interesting]
import mraa.Dir;
import mraa.Gpio;
import mraa.IntelEdison;
import mraa.mraa;
import mraa.Platform;
import mraa.Result;

public class HelloEdison {
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
  public static void main(String argv[]) {
    Platform platform = mraa.getPlatformType();
    
    if (platform != Platform.INTEL_EDISON_FAB_C) {
        System.err.println("Error: This program can only be run on edison");
        System.exit(Result.ERROR_INVALID_PLATFORM.swigValue());
    }
    
    /* 
     * MRAA_INTEL_EDISON_GP182 == 0, so this will initialise pin0 on arduino,
     * which is hardware GPIO 130 and not 182
     * We set the owner to false here, this makes sure that we do not close the
     * gpio from sysfs in mraa_gpio_close meaning it will stay as an output and
     * we will not always transition from 0->1 as gpio182 as output has the
     * default position of '0'. Note that the value could change as a result of
     * a mraa_gpio_dir however meaning we always go from 0->1 or 1->0
     */
    Gpio gpio182 = new Gpio(IntelEdison.INTEL_EDISON_GP182.swigValue(), false);
    gpio182.dir(Dir.DIR_OUT);
    
    int val = gpio182.read();
    
    System.out.println(String.format("GPIO%d (mraa pin %d) was: %d, will set to %d\n", 182,
            gpio182.getPin(), val, val == 0 ? 1 : 0));
    
    gpio182.write(val == 0 ? 1 : 0);
  };
}
//! [Interesting]
