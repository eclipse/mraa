/*
 * Author: Nicola Lunghi <nicola.lunghi@emutex.com>
 * Copyright (c) 2017 Emutex Ltd.
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

#ifndef MRAA_BOARDS_H
#define MRAA_BOARDS_H

#ifdef __cplusplus
extern "C" {
#endif
#include "mraa_internal.h"

/* 
   Function to simply insert a new pin in the information structure mraa_pininfo_t
   Example:

   mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
   ...
   b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * MRAA_UP_PINCOUNT);
   ...
   mraa_set_pininfo(b, 8, "UART1_TX",  (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 }, 14);
*/
mraa_result_t mraa_set_pininfo(mraa_board_t* board, int mraa_index, char* name,
							   mraa_pincapabilities_t caps, int sysfs_pin);

/* 
   Function to get pin index number from name:
   search the pin structure in the board for the given name and write the index 
   of the first match in the *pin_index variable
   Example:

   mraa_get_pin_index_from_name(b, "UART1_RX", &(b->uart_dev[0].rx));
*/
mraa_result_t mraa_get_pin_index_from_name(mraa_board_t* board, char* name, int* pin_index);

#ifdef __cplusplus
}
#endif

#endif /* MRAA_BOARDS_H */
