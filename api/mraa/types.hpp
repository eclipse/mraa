/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright © 2014 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

/** @file
 *
 * This file defines the basic shared types for libmraa
 * this file is different to common.h in that swig takes this as an input
 */

namespace mraa
{

//These enums must match the enums in types.h

/**
 * MRAA supported platform types
 */
typedef enum {
    INTEL_GALILEO_GEN1 = 0,    /**< The Generation 1 Galileo platform (RevD) */
    INTEL_GALILEO_GEN2 = 1,    /**< The Generation 2 Galileo platform (RevG/H) */
    INTEL_EDISON_FAB_C = 2,    /**< The Intel Edison (FAB C) */
    INTEL_DE3815 = 3,          /**< The Intel DE3815 Baytrail NUC */
    INTEL_MINNOWBOARD_MAX = 4, /**< The Intel Minnow Board Max */
    RASPBERRY_PI = 5,          /**< The different Raspberry PI Models -like  A,B,A+,B+ */
    BEAGLEBONE = 6,            /**< The different BeagleBone Black Modes B/C */
    BANANA = 7,                /**< Allwinner A20 based Banana Pi and Banana Pro */
    INTEL_NUC5 = 8,            /**< The Intel 5th generations Broadwell NUCs */
    A96BOARDS = 9,             /**< Linaro 96boards, A prefix for 'ARM' since not allowed numerical */
    INTEL_SOFIA_3GR = 10,      /**< The Intel SoFIA 3GR */
    INTEL_CHERRYHILLS = 11,    /**< The Intel Braswell Cherryhills */
    INTEL_UP = 12,             /**< The UP Board */
    INTEL_JOULE_EXPANSION = 13,/**< The Intel Joule Expansion Board */
    PHYBOARD_WEGA = 14,        /**< The phyBOARD-Wega */
    DE_NANO_SOC = 15,          /**< Terasic DE-Nano-SoC Board */
    INTEL_UP2 = 16,            /**< The UP^2 Board */
    MTK_LINKIT = 17,           /**< Mediatek MT7688 based Linkit boards */
    MTK_OMEGA2 = 18,           /**< MT7688 based Onion Omega2 board */
    IEI_TANK = 19,             /**< IEI Tank System*/
    ROCKPI4 = 20,              /**< Radxa ROCK PI 4 Models A/B/C */
    INTEL_UPXTREME = 24,        /**< The UPXTREME Board */
    SIEMENS_IOT2050 = 26,      /**< Siemens IOT2050 board */
    RADXA_ROCK_3C = 27,        /**< Radxa ROCK 3 Model C */
    VISIONFIVE = 28,           /**< StarFive VisionFive board */
    RADXA_ROCK_5A = 29,        /**< Radxa ROCK 5 Model A */
    RADXA_ROCK_5B = 30,        /**< Radxa ROCK 5 Model B */
    ORANGE_PI_PRIME = 31,      /**< Orange Pi Prime board */
    RADXA_ROCK_3B = 32,        /**< Radxa ROCK 3 Model B */
    RADXA_CM3 = 33,            /**< Radxa CM3 */
    RADXA_CM5_IO = 34,         /**< Radxa CM5 IO */
    RADXA_ROCK_3A = 35,        /**< Radxa ROCK 3 Model A */
    RADXA_E25 = 36,            /**< Radxa E25 */

    FTDI_FT4222 = 256,         /**< FTDI FT4222 USB to i2c bridge */

    GROVEPI = 1024,            /**< GrovePi shield i2c bridge */
    GENERIC_FIRMATA = 1280,    /**< Firmata uart platform/bridge */

    ANDROID_PERIPHERALMANAGER = 95, /**< Android Things peripheral manager platform */
    MOCK_PLATFORM = 96,        /**< Mock platform, which requires no real hardware */
    NULL_PLATFORM = 98,
    UNKNOWN_PLATFORM = 99 /**< An unknown platform type, typically will load INTEL_GALILEO_GEN1 */
} Platform;

/**
 * Intel edison miniboard numbering enum
 */
typedef enum {
    INTEL_EDISON_MINIBOARD_J17_1 = 0,
    INTEL_EDISON_MINIBOARD_J17_5 = 4,
    INTEL_EDISON_MINIBOARD_J17_7 = 6,
    INTEL_EDISON_MINIBOARD_J17_8 = 7,
    INTEL_EDISON_MINIBOARD_J17_9 = 8,
    INTEL_EDISON_MINIBOARD_J17_10 = 9,
    INTEL_EDISON_MINIBOARD_J17_11 = 10,
    INTEL_EDISON_MINIBOARD_J17_12 = 11,
    INTEL_EDISON_MINIBOARD_J17_14 = 13,
    INTEL_EDISON_MINIBOARD_J18_1 = 14,
    INTEL_EDISON_MINIBOARD_J18_2 = 15,
    INTEL_EDISON_MINIBOARD_J18_6 = 19,
    INTEL_EDISON_MINIBOARD_J18_7 = 20,
    INTEL_EDISON_MINIBOARD_J18_8 = 21,
    INTEL_EDISON_MINIBOARD_J18_10 = 23,
    INTEL_EDISON_MINIBOARD_J18_11 = 24,
    INTEL_EDISON_MINIBOARD_J18_12 = 25,
    INTEL_EDISON_MINIBOARD_J18_13 = 26,
    INTEL_EDISON_MINIBOARD_J19_4 = 31,
    INTEL_EDISON_MINIBOARD_J19_5 = 32,
    INTEL_EDISON_MINIBOARD_J19_6 = 33,
    INTEL_EDISON_MINIBOARD_J19_8 = 35,
    INTEL_EDISON_MINIBOARD_J19_9 = 36,
    INTEL_EDISON_MINIBOARD_J19_10 = 37,
    INTEL_EDISON_MINIBOARD_J19_11 = 38,
    INTEL_EDISON_MINIBOARD_J19_12 = 39,
    INTEL_EDISON_MINIBOARD_J19_13 = 40,
    INTEL_EDISON_MINIBOARD_J19_14 = 41,
    INTEL_EDISON_MINIBOARD_J20_3 = 44,
    INTEL_EDISON_MINIBOARD_J20_4 = 45,
    INTEL_EDISON_MINIBOARD_J20_5 = 46,
    INTEL_EDISON_MINIBOARD_J20_6 = 47,
    INTEL_EDISON_MINIBOARD_J20_7 = 48,
    INTEL_EDISON_MINIBOARD_J20_8 = 49,
    INTEL_EDISON_MINIBOARD_J20_9 = 50,
    INTEL_EDISON_MINIBOARD_J20_10 = 51,
    INTEL_EDISON_MINIBOARD_J20_11 = 52,
    INTEL_EDISON_MINIBOARD_J20_12 = 53,
    INTEL_EDISON_MINIBOARD_J20_13 = 54,
    INTEL_EDISON_MINIBOARD_J20_14 = 55
} IntelEdisonMiniboard;

/**
 * Intel Edison raw GPIO numbering enum
 */
typedef enum {
    INTEL_EDISON_GP182 = 0,
    INTEL_EDISON_GP135 = 4,
    INTEL_EDISON_GP27 = 6,
    INTEL_EDISON_GP20 = 7,
    INTEL_EDISON_GP28 = 8,
    INTEL_EDISON_GP111 = 0,
    INTEL_EDISON_GP109 = 10,
    INTEL_EDISON_GP115 = 11,
    INTEL_EDISON_GP128 = 13,
    INTEL_EDISON_GP13 = 14,
    INTEL_EDISON_GP165 = 15,
    INTEL_EDISON_GP19 = 19,
    INTEL_EDISON_GP12 = 20,
    INTEL_EDISON_GP183 = 21,
    INTEL_EDISON_GP110 = 23,
    INTEL_EDISON_GP114 = 24,
    INTEL_EDISON_GP129 = 25,
    INTEL_EDISON_GP130 = 26,
    INTEL_EDISON_GP44 = 31,
    INTEL_EDISON_GP46 = 32,
    INTEL_EDISON_GP48 = 33,
    INTEL_EDISON_GP131 = 35,
    INTEL_EDISON_GP14 = 36,
    INTEL_EDISON_GP40 = 37,
    INTEL_EDISON_GP43 = 38,
    INTEL_EDISON_GP77 = 39,
    INTEL_EDISON_GP82 = 40,
    INTEL_EDISON_GP83 = 41,
    INTEL_EDISON_GP134 = 44,
    INTEL_EDISON_GP45 = 45,
    INTEL_EDISON_GP47 = 46,
    INTEL_EDISON_GP49 = 47,
    INTEL_EDISON_GP15 = 48,
    INTEL_EDISON_GP84 = 49,
    INTEL_EDISON_GP42 = 50,
    INTEL_EDISON_GP41 = 51,
    INTEL_EDISON_GP78 = 52,
    INTEL_EDISON_GP79 = 53,
    INTEL_EDISON_GP80 = 54,
    INTEL_EDISON_GP81 = 55
} IntelEdison;

/**
 * Radxa ROCK 5 Model A GPIO numbering enum
 */
typedef enum {
    RADXA_ROCK_5A_PIN3 = 3,
    RADXA_ROCK_5A_PIN5 = 5,
    RADXA_ROCK_5A_PIN7 = 7,
    RADXA_ROCK_5A_PIN8 = 8,
    RADXA_ROCK_5A_PIN10 = 10,
    RADXA_ROCK_5A_PIN11 = 11,
    RADXA_ROCK_5A_PIN12 = 12,
    RADXA_ROCK_5A_PIN13 = 13,
    RADXA_ROCK_5A_PIN15 = 15,
    RADXA_ROCK_5A_PIN16 = 16,
    RADXA_ROCK_5A_PIN18 = 18,
    RADXA_ROCK_5A_PIN19 = 19,
    RADXA_ROCK_5A_PIN21 = 21,
    RADXA_ROCK_5A_PIN22 = 22,
    RADXA_ROCK_5A_PIN23 = 23,
    RADXA_ROCK_5A_PIN24 = 24,
    RADXA_ROCK_5A_PIN26 = 26,
    RADXA_ROCK_5A_PIN27 = 27,
    RADXA_ROCK_5A_PIN28 = 28,
    RADXA_ROCK_5A_PIN29 = 29,
    RADXA_ROCK_5A_PIN31 = 31,
    RADXA_ROCK_5A_PIN32 = 32,
    RADXA_ROCK_5A_PIN33 = 33,
    RADXA_ROCK_5A_PIN35 = 35,
    RADXA_ROCK_5A_PIN36 = 36,
    RADXA_ROCK_5A_PIN37 = 37,
    RADXA_ROCK_5A_PIN38 = 38,
    RADXA_ROCK_5A_PIN40 = 40
} RadxaRock5AWiring;

/**
 * Radxa ROCK 5 Model B GPIO numbering enum
 */
typedef enum {
    RADXA_ROCK_5B_PIN3 = 3,
    RADXA_ROCK_5B_PIN5 = 5,
    RADXA_ROCK_5B_PIN7 = 7,
    RADXA_ROCK_5B_PIN8 = 8,
    RADXA_ROCK_5B_PIN10 = 10,
    RADXA_ROCK_5B_PIN11 = 11,
    RADXA_ROCK_5B_PIN12 = 12,
    RADXA_ROCK_5B_PIN13 = 13,
    RADXA_ROCK_5B_PIN15 = 15,
    RADXA_ROCK_5B_PIN16 = 16,
    RADXA_ROCK_5B_PIN18 = 18,
    RADXA_ROCK_5B_PIN19 = 19,
    RADXA_ROCK_5B_PIN21 = 21,
    RADXA_ROCK_5B_PIN22 = 22,
    RADXA_ROCK_5B_PIN23 = 23,
    RADXA_ROCK_5B_PIN24 = 24,
    RADXA_ROCK_5B_PIN26 = 26,
    RADXA_ROCK_5B_PIN27 = 27,
    RADXA_ROCK_5B_PIN28 = 28,
    RADXA_ROCK_5B_PIN29 = 29,
    RADXA_ROCK_5B_PIN31 = 31,
    RADXA_ROCK_5B_PIN32 = 32,
    RADXA_ROCK_5B_PIN33 = 33,
    RADXA_ROCK_5B_PIN35 = 35,
    RADXA_ROCK_5B_PIN36 = 36,
    RADXA_ROCK_5B_PIN37 = 37,
    RADXA_ROCK_5B_PIN38 = 38,
    RADXA_ROCK_5B_PIN40 = 40
} RadxaRock5BWiring;

/**
 * Radxa CM5 IO GPIO numbering enum
 */
typedef enum {
    RADXA_CM5_IO_PIN3 = 3,
    RADXA_CM5_IO_PIN5 = 5,
    RADXA_CM5_IO_PIN7 = 7,
    RADXA_CM5_IO_PIN8 = 8,
    RADXA_CM5_IO_PIN10 = 10,
    RADXA_CM5_IO_PIN11 = 11,
    RADXA_CM5_IO_PIN12 = 12,
    RADXA_CM5_IO_PIN13 = 13,
    RADXA_CM5_IO_PIN15 = 15,
    RADXA_CM5_IO_PIN16 = 16,
    RADXA_CM5_IO_PIN18 = 18,
    RADXA_CM5_IO_PIN19 = 19,
    RADXA_CM5_IO_PIN21 = 21,
    RADXA_CM5_IO_PIN22 = 22,
    RADXA_CM5_IO_PIN23 = 23,
    RADXA_CM5_IO_PIN24 = 24,
    RADXA_CM5_IO_PIN26 = 26,
    RADXA_CM5_IO_PIN29 = 29,
    RADXA_CM5_IO_PIN31 = 31,
    RADXA_CM5_IO_PIN32 = 32,
    RADXA_CM5_IO_PIN33 = 33,
    RADXA_CM5_IO_PIN35 = 35,
    RADXA_CM5_IO_PIN36 = 36,
    RADXA_CM5_IO_PIN38 = 38,
    RADXA_CM5_IO_PIN40 = 40
} RadxaCM5IOWiring;

/**
 *  Radxa ROCK 3 Model A GPIO numbering enum
 */
typedef enum {
    RADXA_ROCK_3A_PIN3 = 3,
    RADXA_ROCK_3A_PIN5 = 5,
    RADXA_ROCK_3A_PIN7 = 7,
    RADXA_ROCK_3A_PIN8 = 8,
    RADXA_ROCK_3A_PIN10 = 10,
    RADXA_ROCK_3A_PIN11 = 11,
    RADXA_ROCK_3A_PIN12 = 12,
    RADXA_ROCK_3A_PIN13 = 13,
    RADXA_ROCK_3A_PIN15 = 15,
    RADXA_ROCK_3A_PIN16 = 16,
    RADXA_ROCK_3A_PIN17 = 17,
    RADXA_ROCK_3A_PIN18 = 18,
    RADXA_ROCK_3A_PIN19 = 19,
    RADXA_ROCK_3A_PIN21 = 21,
    RADXA_ROCK_3A_PIN23 = 23,
    RADXA_ROCK_3A_PIN24 = 24,
    RADXA_ROCK_3A_PIN26 = 26,
    RADXA_ROCK_3A_PIN27 = 27,
    RADXA_ROCK_3A_PIN28 = 28,
    RADXA_ROCK_3A_PIN29 = 29,
    RADXA_ROCK_3A_PIN31 = 31,
    RADXA_ROCK_3A_PIN32 = 32,
    RADXA_ROCK_3A_PIN33 = 33,
    RADXA_ROCK_3A_PIN35 = 35,
    RADXA_ROCK_3A_PIN36 = 36,
    RADXA_ROCK_3A_PIN37 = 37,
    RADXA_ROCK_3A_PIN38 = 38,
    RADXA_ROCK_3A_PIN40 = 40
} RadxaRock3AWiring;

/**
 *  Radxa ROCK 3 Model B GPIO numbering enum
 */
typedef enum {
    RADXA_ROCK_3B_PIN3 = 3,
    RADXA_ROCK_3B_PIN5 = 5,
    RADXA_ROCK_3B_PIN7 = 7,
    RADXA_ROCK_3B_PIN8 = 8,
    RADXA_ROCK_3B_PIN10 = 10,
    RADXA_ROCK_3B_PIN11 = 11,
    RADXA_ROCK_3B_PIN12 = 12,
    RADXA_ROCK_3B_PIN13 = 13,
    RADXA_ROCK_3B_PIN15 = 15,
    RADXA_ROCK_3B_PIN16 = 16,
    RADXA_ROCK_3B_PIN18 = 18,
    RADXA_ROCK_3B_PIN19 = 19,
    RADXA_ROCK_3B_PIN21 = 21,
    RADXA_ROCK_3B_PIN22 = 22,
    RADXA_ROCK_3B_PIN23 = 23,
    RADXA_ROCK_3B_PIN24 = 24,
    RADXA_ROCK_3B_PIN27 = 27,
    RADXA_ROCK_3B_PIN28 = 28,
    RADXA_ROCK_3B_PIN29 = 29,
    RADXA_ROCK_3B_PIN31 = 31,
    RADXA_ROCK_3B_PIN32 = 32,
    RADXA_ROCK_3B_PIN33 = 33,
    RADXA_ROCK_3B_PIN35 = 35,
    RADXA_ROCK_3B_PIN36 = 36,
    RADXA_ROCK_3B_PIN38 = 38,
    RADXA_ROCK_3B_PIN40 = 40
} RadxaRock3BWiring;

/**
 *  Radxa ROCK 3 Model C GPIO numbering enum
 */
typedef enum {
    RADXA_ROCK_3C_PIN3 = 3,
    RADXA_ROCK_3C_PIN5 = 5,
    RADXA_ROCK_3C_PIN7 = 7,
    RADXA_ROCK_3C_PIN8 = 8,
    RADXA_ROCK_3C_PIN10 = 10,
    RADXA_ROCK_3C_PIN11 = 11,
    RADXA_ROCK_3C_PIN12 = 12,
    RADXA_ROCK_3C_PIN13 = 13,
    RADXA_ROCK_3C_PIN15 = 15,
    RADXA_ROCK_3C_PIN16 = 16,
    RADXA_ROCK_3C_PIN18 = 18,
    RADXA_ROCK_3C_PIN19 = 19,
    RADXA_ROCK_3C_PIN21 = 21,
    RADXA_ROCK_3C_PIN22 = 22,
    RADXA_ROCK_3C_PIN23 = 23,
    RADXA_ROCK_3C_PIN24 = 24,
    RADXA_ROCK_3C_PIN27 = 27,
    RADXA_ROCK_3C_PIN28 = 28,
    RADXA_ROCK_3C_PIN29 = 29,
    RADXA_ROCK_3C_PIN31 = 31,
    RADXA_ROCK_3C_PIN32 = 32,
    RADXA_ROCK_3C_PIN33 = 33,
    RADXA_ROCK_3C_PIN35 = 35,
    RADXA_ROCK_3C_PIN36 = 36,
    RADXA_ROCK_3C_PIN37 = 37,
    RADXA_ROCK_3C_PIN38 = 38,
    RADXA_ROCK_3C_PIN40 = 40
} RadxaRock3CWiring;

/**
 *  Radxa CM3 IO GPIO numbering enum
 */
typedef enum {
    RADXA_CM3_IO_PIN3 = 3,
    RADXA_CM3_IO_PIN5 = 5,
    RADXA_CM3_IO_PIN7 = 7,
    RADXA_CM3_IO_PIN8 = 8,
    RADXA_CM3_IO_PIN10 = 10,
    RADXA_CM3_IO_PIN11 = 11,
    RADXA_CM3_IO_PIN12 = 12,
    RADXA_CM3_IO_PIN13 = 13,
    RADXA_CM3_IO_PIN15 = 15,
    RADXA_CM3_IO_PIN16 = 16,
    RADXA_CM3_IO_PIN18 = 18,
    RADXA_CM3_IO_PIN19 = 19,
    RADXA_CM3_IO_PIN21 = 21,
    RADXA_CM3_IO_PIN22 = 22,
    RADXA_CM3_IO_PIN23 = 23,
    RADXA_CM3_IO_PIN24 = 24,
    RADXA_CM3_IO_PIN27 = 27,
    RADXA_CM3_IO_PIN28 = 28,
    RADXA_CM3_IO_PIN29 = 29,
    RADXA_CM3_IO_PIN31 = 31,
    RADXA_CM3_IO_PIN32 = 32,
    RADXA_CM3_IO_PIN33 = 33,
    RADXA_CM3_IO_PIN35 = 35,
    RADXA_CM3_IO_PIN36 = 36,
    RADXA_CM3_IO_PIN37 = 37,
    RADXA_CM3_IO_PIN38 = 38,
    RADXA_CM3_IO_PIN40 = 40
} RadxaCM3IOWiring;

/**
 *  Radxa E25 GPIO numbering enum
 */
typedef enum {
    RADXA_E25_PIN3 = 3,
    RADXA_E25_PIN5 = 5,
    RADXA_E25_PIN7 = 7,
    RADXA_E25_PIN8 = 8,
    RADXA_E25_PIN10 = 10,
    RADXA_E25_PIN11 = 11,
    RADXA_E25_PIN12 = 12,
    RADXA_E25_PIN13 = 13,
    RADXA_E25_PIN15 = 15,
    RADXA_E25_PIN16 = 16,
    RADXA_E25_PIN17 = 17,
    RADXA_E25_PIN18 = 18,
    RADXA_E25_PIN19 = 19,
    RADXA_E25_PIN21 = 21,
    RADXA_E25_PIN22 = 22,
    RADXA_E25_PIN23 = 23,
    RADXA_E25_PIN24 = 24,
    RADXA_E25_PIN26 = 26
} RadxaE25Wiring;

/**
 *  ROCKPI4 GPIO numbering enum
 */
typedef enum {
    ROCKPI4_PIN3 = 3,
    ROCKPI4_PIN5 = 5,
    ROCKPI4_PIN7 = 7,
    ROCKPI4_PIN8 = 8,
    ROCKPI4_PIN10 = 10,
    ROCKPI4_PIN11 = 11,
    ROCKPI4_PIN12 = 12,
    ROCKPI4_PIN13 = 13,
    ROCKPI4_PIN15 = 15,
    ROCKPI4_PIN16 = 16,
    ROCKPI4_PIN18 = 18,
    ROCKPI4_PIN19 = 19,
    ROCKPI4_PIN21 = 21,
    ROCKPI4_PIN22 = 22,
    ROCKPI4_PIN23 = 23,
    ROCKPI4_PIN24 = 24,
    ROCKPI4_PIN27 = 27,
    ROCKPI4_PIN28 = 28,
    ROCKPI4_PIN29 = 29,
    ROCKPI4_PIN31 = 31,
    ROCKPI4_PIN32 = 32,
    ROCKPI4_PIN33 = 33,
    ROCKPI4_PIN35 = 35,
    ROCKPI4_PIN36 = 36,
    ROCKPI4_PIN37 = 37,
    ROCKPI4_PIN38 = 38,
    ROCKPI4_PIN40 = 40
} RockPi4Wiring;

/**
* Raspberry PI Wiring compatible numbering enum
*/
typedef enum {
    RASPBERRY_WIRING_PIN8 = 3,
    RASPBERRY_WIRING_PIN9 = 5,
    RASPBERRY_WIRING_PIN7 = 7,
    RASPBERRY_WIRING_PIN15 = 8,
    RASPBERRY_WIRING_PIN16 = 10,
    RASPBERRY_WIRING_PIN0 = 11,
    RASPBERRY_WIRING_PIN1 = 12,
    RASPBERRY_WIRING_PIN2 = 13,
    RASPBERRY_WIRING_PIN3 = 15,
    RASPBERRY_WIRING_PIN4 = 16,
    RASPBERRY_WIRING_PIN5 = 18,
    RASPBERRY_WIRING_PIN12 = 19,
    RASPBERRY_WIRING_PIN13 = 21,
    RASPBERRY_WIRING_PIN6 = 22,
    RASPBERRY_WIRING_PIN14 = 23,
    RASPBERRY_WIRING_PIN10 = 24,
    RASPBERRY_WIRING_PIN11 = 26,
    RASPBERRY_WIRING_PIN17 = 29, // RPi B V2
    RASPBERRY_WIRING_PIN21 = 29,
    RASPBERRY_WIRING_PIN18 = 30, // RPi B V2
    RASPBERRY_WIRING_PIN19 = 31, // RPI B V2
    RASPBERRY_WIRING_PIN22 = 31,
    RASPBERRY_WIRING_PIN20 = 32, // RPi B V2
    RASPBERRY_WIRING_PIN26 = 32,
    RASPBERRY_WIRING_PIN23 = 33,
    RASPBERRY_WIRING_PIN24 = 35,
    RASPBERRY_WIRING_PIN27 = 36,
    RASPBERRY_WIRING_PIN25 = 37,
    RASPBERRY_WIRING_PIN28 = 38,
    RASPBERRY_WIRING_PIN29 = 40
} RaspberryWiring;

/**
 *  Orange Pi Prime GPIO numbering enum
 */
typedef enum {
    ORANGE_PI_PRIME_PIN3 = 3,
    ORANGE_PI_PRIME_PIN5 = 5,
    ORANGE_PI_PRIME_PIN7 = 7,
    ORANGE_PI_PRIME_PIN8 = 8,
    ORANGE_PI_PRIME_PIN10 = 10,
    ORANGE_PI_PRIME_PIN11 = 11,
    ORANGE_PI_PRIME_PIN12 = 12,
    ORANGE_PI_PRIME_PIN13 = 13,
    ORANGE_PI_PRIME_PIN15 = 15,
    ORANGE_PI_PRIME_PIN16 = 16,
    ORANGE_PI_PRIME_PIN18 = 18,
    ORANGE_PI_PRIME_PIN19 = 19,
    ORANGE_PI_PRIME_PIN21 = 21,
    ORANGE_PI_PRIME_PIN22 = 22,
    ORANGE_PI_PRIME_PIN23 = 23,
    ORANGE_PI_PRIME_PIN24 = 24,
    ORANGE_PI_PRIME_PIN26 = 26,
    ORANGE_PI_PRIME_PIN27 = 27,
    ORANGE_PI_PRIME_PIN28 = 28,
    ORANGE_PI_PRIME_PIN29 = 29,
    ORANGE_PI_PRIME_PIN31 = 31,
    ORANGE_PI_PRIME_PIN32 = 32,
    ORANGE_PI_PRIME_PIN33 = 33,
    ORANGE_PI_PRIME_PIN35 = 35,
    ORANGE_PI_PRIME_PIN36 = 36,
    ORANGE_PI_PRIME_PIN37 = 37,
    ORANGE_PI_PRIME_PIN38 = 38,
    ORANGE_PI_PRIME_PIN40 = 40
} OrangePiPrimeWiring;

/**
 * MRAA return codes
 */
typedef enum {
    SUCCESS = 0,                             /**< Expected response */
    ERROR_FEATURE_NOT_IMPLEMENTED = 1,       /**< Feature TODO */
    ERROR_FEATURE_NOT_SUPPORTED = 2,         /**< Feature not supported by HW */
    ERROR_INVALID_VERBOSITY_LEVEL = 3,       /**< Verbosity level wrong */
    ERROR_INVALID_PARAMETER = 4,             /**< Parameter invalid */
    ERROR_INVALID_HANDLE = 5,                /**< Handle invalid */
    ERROR_NO_RESOURCES = 6,                  /**< No resource of that type avail */
    ERROR_INVALID_RESOURCE = 7,              /**< Resource invalid */
    ERROR_INVALID_QUEUE_TYPE = 8,            /**< Queue type incorrect */
    ERROR_NO_DATA_AVAILABLE = 9,             /**< No data available */
    ERROR_INVALID_PLATFORM = 10,             /**< Platform not recognised */
    ERROR_PLATFORM_NOT_INITIALISED = 11,     /**< Board information not initialised */
    ERROR_UART_OW_SHORTED = 12,              /**< UART OW Short Circuit Detected*/
    ERROR_UART_OW_NO_DEVICES = 13,           /**< UART OW No devices detected */
    ERROR_UART_OW_DATA_ERROR = 14,           /**< UART OW Data/Bus error detected */

    ERROR_UNSPECIFIED = 99 /**< Unknown Error */
} Result;

/**
 * Enum representing different possible modes for a pin.
 */
typedef enum {
    PIN_VALID = 0,     /**< Pin Valid */
    PIN_GPIO = 1,      /**< General Purpose IO */
    PIN_PWM = 2,       /**< Pulse Width Modulation */
    PIN_FAST_GPIO = 3, /**< Faster GPIO */
    PIN_SPI = 4,       /**< SPI */
    PIN_I2C = 5,       /**< I2C */
    PIN_AIO = 6,       /**< Analog in */
    PIN_UART = 7       /**< UART */
} Pinmodes;

/**
 * Enum representing different i2c speeds/modes
 */
typedef enum {
    I2C_STD = 0,  /**< up to 100Khz */
    I2C_FAST = 1, /**< up to 400Khz */
    I2C_HIGH = 2  /**< up to 3.4Mhz */
} I2cMode;

/**
 * Enum representing different uart parity states
 */
typedef enum {
    UART_PARITY_NONE = 0,
    UART_PARITY_EVEN = 1,
    UART_PARITY_ODD = 2,
    UART_PARITY_MARK = 3,
    UART_PARITY_SPACE = 4
} UartParity;

}
