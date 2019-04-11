/*
 * Author: Mihai Tudor Panu <mihai.tudor.panu@intel.com>
 * Copyright (c) 2018 Intel Corporation
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

#include <iostream>
#include <string>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "mraa/common.hpp"
#include "mraa/led.hpp"

// Define min and max brightness
#define LED_ON  255
#define LED_OFF 0

// This boolean controls the main loop
volatile bool should_run = true;

// Interrupt signal handler function
void
sig_handler(int signum)
{
    if (signum == SIGINT) {
        std::cout << "Exiting..." << std::endl;
        should_run = false;
    }
}

// Main function
int
main()
{
    // Perform a basic platform and version check
    if (mraa::getPlatformType() != mraa::INTEL_UP2) {
        std::cout << "This example is meant for the UP Squared board." << std::endl;
        std::cout << "Running it on different platforms will likely require code changes!" << std::endl;
    }

    if (mraa::getVersion().compare("v1.9.0") < 0) {
        std::cout << "You need MRAA version 1.9.0 or newer to run this sample!" << std::endl;
        return 1;
    }

    // Register handler for keyboard interrupts
    signal(SIGINT, sig_handler);

    // Define our LEDs
	std::string led_names[] = {"red", "green", "yellow", "blue"};
    int led_no = sizeof(led_names)/sizeof(led_names[0]);
    mraa::Led **leds = new mraa::Led*[led_no];

    // Initialize them
    for (int i = 0; i < led_no; i++) {
        leds[i] = new mraa::Led(led_names[i].c_str());
    }

    // Blink them in order and pause in-between
    while (should_run) {
        for (int i = 0; i < led_no; i++) {
            leds[i]->setBrightness(LED_ON);
            usleep(200000);
            leds[i]->setBrightness(LED_OFF);
        }
        usleep(200000);
    }

    // Cleanup and exit
    for (int i = 0; i < led_no; i++) {
        delete leds[i];
    }
    delete [] leds;
    return 0;
}
