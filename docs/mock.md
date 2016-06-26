Mock platform   {#mock}
=============

Mocking capability allows libmraa user to work with the library without any real
hardware available. Enabling this requires library recompilation with architecture
override (see Building section below). When mock is enabled, library simulates
actual HW operations at the backend so that for the application it looks
like a usual board. Being implemented at the backend, the functionality is available
in all language bindings libmraa supports.

Board configuration
-------------------

This feature is yet in the experimental mode and not all functionality is available.
Right now we simulate a single generic board with only GPIO (without ISR) working.
It also reports having an ADC with 10 (std)/12 (max) bit resolution, but the ADC
functionality itself is not yet implemented.

We plan to develop it further and all [contributions](../CONTRIBUTING.md) are more than welcome.

See the table below for pin layout and features

| MRAA Number | Pin Name |            Notes            |
|-------------|----------|-----------------------------|
| 0           | GPIO0    | GPIO pin, no muxing, no ISR |

Building
--------

Generally all the building steps are the same as listed
in the [main building guide](./building.md), you just need to set some specific
CMake options.

### Linux

To build under Linux, follow standard instructions, just make sure to set
the `-DBUILDARCH="MOCK"` CMake option.
