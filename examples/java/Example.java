/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 * Author: Petre Eftime <petre.p.eftime@intel.com>
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

//! [Interesting]
import mraa.mraa;

public class Example {
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
    String board = mraa.getPlatformName();
    String version = mraa.getVersion();
    System.out.println("hello mraa");
    System.out.println(String.format("Version: %s", version));
    System.out.println(String.format("Running on %s", board));
  };
}
//! [Interesting]
