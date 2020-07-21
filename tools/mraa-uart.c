/*
 * Author: Tapani Utriainen <tapani@technexion.com>
 * Copyright (c) 2017 TechNexion Ltd.
 *
 * SPDX-License-Identifier: MIT
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "mraa.h"

#ifndef FALSE
#define FALSE 0
#define TRUE (!FALSE)
#endif

static const char *undefined = "<undefined>";

static const char paritymode_table[] = {
   [MRAA_UART_PARITY_NONE] = 'N',
   [MRAA_UART_PARITY_EVEN] = 'E',
   [MRAA_UART_PARITY_ODD] = 'O',
   [MRAA_UART_PARITY_MARK] = 'M',
   [MRAA_UART_PARITY_SPACE] = 'S'
};

/* ------------------------------------------------------------------------ */

mraa_uart_context
uarttool_find(const char *str) {
    /* Try to find the uart by name, then by index, by devpath and as last resort create a raw uart */
    int uart_index = mraa_uart_lookup(str);
    if (uart_index < 0) {
        /* name lookup failed, try index if the first character is a digit */
        if (isdigit(*str)) {
            uart_index = atoi(str);
        } else {
            const char *devpath;
            int number_of_uarts = mraa_get_uart_count();
            for (uart_index = 0; uart_index < number_of_uarts; uart_index++) {
                 mraa_result_t res = mraa_uart_settings(uart_index, NULL, &devpath, NULL, NULL, NULL, NULL, NULL, NULL);
                 if (res == MRAA_SUCCESS && devpath != NULL && !strcmp(devpath, str)) break;
            }
            if (uart_index >= number_of_uarts) {
                 return mraa_uart_init_raw(str);
            }
        }
    }

    return mraa_uart_init(uart_index);
}

/* ------------------------------------------------------------------------ */

double
now(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_usec * 1e-6 + (double)tv.tv_sec;
}

/* ------------------------------------------------------------------------ */

void
uarttool_read_and_print(mraa_uart_context uart, double receive_timeout) {
   char buf;
   do {
       double start = now();
       if (mraa_uart_data_available(uart, 1.0 + receive_timeout * 1000.0)) {
           mraa_uart_read(uart, &buf, 1);
           putchar(buf);
           fflush(stdout);
       }
       receive_timeout -= now() - start;
   } while (receive_timeout > 0.0);
}

/* ------------------------------------------------------------------------ */

void
uarttool_usage(const char *name) {
     printf("Usage: %s { list | dev device } [ baud bps ] [ databits d ] [ parity p ] [ stopbits s ] [ ctsrts mode ] [ send string ] [ recv timeout ] [ show ]\n\n", name);
     printf("Simple tool to test UART functionality. Needs either list or dev arguments, the others are optional\n");
     printf("   list     : lists uarts on the system (non intrusive)\n");
     printf("   dev      : select uart device, can be by name, by device name or by index (as listed in list)\n");
     printf("   baud     : set the baudrate to given parameter, in bits per second\n");
     printf("   parity   : set parity mode to given parameter - can be E, O, or N\n");
     printf("   stopbits : set the number of stopbits - can be 1 or 2\n");
     printf("   ctsrts   : set CTS/RTS flow control to either on or off\n");
     printf("   send     : transmits a string\n");
     printf("   recv     : reads data on uart for timeout seconds, and displays the result on stdout\n");
     printf("   show     : show settings of selected uart\n");
}

/* ------------------------------------------------------------------------ */

int
main(int argc, const char** argv) {
    mraa_uart_context uart = NULL;
    int uart_index, i;

    int baudrate = 115200, stopbits = 1, databits = 8;
    mraa_uart_parity_t parity = MRAA_UART_PARITY_NONE;
    unsigned int ctsrts = FALSE, xonxoff = FALSE;
    const char *name = NULL, *dev = NULL;

    double recieve_timeout = 0.0;
    int recieve = FALSE; /* whether we are requested to receive */
    int send = FALSE; /* whether we are requested to send or not */
    const char *to_send; /* data to send, assigned during parsing of command line */
    int show = FALSE; /* whether to show uart settings after everything else*/

    /* Initialize MRAA. Init is done automatically if libmraa is compiled
       with a compiler that supports __attribute__((constructor)), like
       GCC and CLANG
    */
    mraa_init();

    if (argc <= 1) {
        uarttool_usage(argv[0]);
    } else
    if (!strcmp(argv[1], "list")) {
        int number_of_uarts = mraa_get_uart_count();
        for (uart_index = 0; uart_index < number_of_uarts; uart_index++) {
             mraa_result_t res = mraa_uart_settings(uart_index, &name, &dev, &baudrate, &databits, &stopbits, &parity, &ctsrts, &xonxoff);
             if (res == MRAA_SUCCESS) {
                 if (name == NULL) name = undefined;
                 if (dev == NULL) dev = undefined;
                 printf("%i. %-12s %-16s %7i %i%c%i %s %s\n", uart_index, name, dev, baudrate, databits, paritymode_table[parity], stopbits, ctsrts?"CTSRTS":"(none)", xonxoff?"XONXOFF":"(none) ");
             } else {
                 printf("%i. ---- error reading uart information ----\n", uart_index);
             }
        }
    } else {
        for (i=1; i<argc; i++) {
            if (!strcmp(argv[i], "dev")) {
                uart = uarttool_find(argv[i+1]);
                if (uart == NULL) {
                    fprintf(stderr, "%s : cannot find uart %s\n", argv[0], argv[i+1]);
                    break;
                }
                dev = mraa_uart_get_dev_path(uart);
                mraa_result_t res = mraa_uart_settings(-1, &dev, &name, &baudrate, &databits, &stopbits, &parity, &ctsrts, &xonxoff);
                if (res != MRAA_SUCCESS) {
                    fprintf(stderr, "warning: problems accessing uart settings, attempting to continue\n");
                }
            } else

            /* Baudrate setting */
            if (!strcmp(argv[i], "baud") || !strcmp(argv[i], "baudrate")) {
                if (i+1 >= argc || !isdigit(argv[i+1][0])) {
                    fprintf(stderr, "%s : %s needs number as argument\n", argv[0], argv[i]);
                    break;
                }
                baudrate = atoi(argv[i+1]);
                if (uart != NULL) {
                    mraa_uart_set_baudrate(uart, baudrate);
                }
                i++;
            } else

            /* Accept a few variants of the flow control setting name */
            if (!strcmp(argv[i], "ctsrts") || !strcmp(argv[i], "rtscts") || !strcmp(argv[i], "crtscts")) {
                if (i+1 >= argc || argv[i+1][0] != 'o') {
                    fprintf(stderr, "%s : ctsrts needs either on or off as argument\n", argv[0]);
                    break;
                }
                ctsrts = !strcmp(argv[i+1], "on") ? 1u : 0u;
                xonxoff = 0u;
                if (uart != NULL) {
                    mraa_uart_set_flowcontrol(uart, xonxoff, ctsrts);
                }
                i++;
            } else

            /* Number of stopbits */
            if (!strcmp(argv[i], "stopbits")) {
                if (i+1 >= argc || !isdigit(argv[i+1][0])) {
                    fprintf(stderr, "%s : %s needs number as argument\n", argv[0], argv[i]);
                    break;
                }
                stopbits = atoi(argv[i+1]);
                if (stopbits < 1 || stopbits > 2) {
                    stopbits = 1;
                    fprintf(stderr, "%s : invalid number of stopbits, bust be 1 or 2\n", argv[0]);
                }
                if (uart != NULL) {
                    mraa_uart_set_mode(uart, databits, parity, stopbits);
                }
                i++;
            } else

            /* Databits */
            if (!strcmp(argv[i], "databits") || !strcmp(argv[i], "bytesize")) {
                if (i+1 >= argc || !isdigit(argv[i+1][0])) {
                    fprintf(stderr, "%s : %s needs number as argument\n", argv[0], argv[i]);
                    break;
                }
                databits = atoi(argv[i+1]);
                if (uart != NULL) {
                    mraa_uart_set_mode(uart, databits, parity, stopbits);
                }
                i++;
            } else

            /* Parity mode, uses paritmode array */
            if (!strcmp(argv[i], "paritybits") || !strcmp(argv[i], "parity")) {
                int j;
                if (i+1 >= argc) {
                    fprintf(stderr, "%s : %s needs 'O','E' or 'N' as argument\n", argv[0], argv[i]);
                    break;
                }
                /* Even if there were gaps between declared array elements, they would be initialized to 0 compile time */
                for (j=0; j<sizeof(paritymode_table); j++) {
                   if (paritymode_table[j] == toupper(argv[i+1][0])) {
                       parity = j;
                       break;
                   }
                }
                if (j >= sizeof(paritymode_table)) {
                   fprintf(stderr, "warning: unrecognized parity mode %s (ignoring)\n", argv[i+1]);
                }
                if (uart != NULL) {
                    mraa_uart_set_mode(uart, databits, parity, stopbits);
                }
                i++;
            } else

            /* Show settings */
            if (!strcmp(argv[i], "show")) {
                 show = TRUE;
            } else

            /* Send data */
            if (!strcmp(argv[i], "send")) {
                if (i+1 >= argc) {
                    fprintf(stderr, "%s : %s needs string as argument\n", argv[0], argv[i]);
                    break;
                }
                send = TRUE;
                to_send = argv[i+1];
                i++;
            } else

            if (!strcmp(argv[i], "receive") || !strcmp(argv[i], "recieve") || !strcmp(argv[i], "recv")) {
                if (i+1 >= argc) {
                    fprintf(stderr, "%s : %s needs a timeout (in seconds, fractional ok) as argument\n", argv[0], argv[i]);
                    break;
                }
                recieve = TRUE;
                recieve_timeout = atof(argv[i+1]);
                i++;
            }
        }

        if (i == argc && uart != NULL) {
            if (send) {
                mraa_uart_write(uart, to_send, strlen(to_send)+1);
            }

            if (recieve) {
                uarttool_read_and_print(uart, recieve_timeout);
            }

            if (show) {
                if (recieve) putchar('\n');
                dev = mraa_uart_get_dev_path(uart);
                mraa_uart_settings(-1, &dev, &name, &baudrate, &databits, &stopbits, &parity, &ctsrts, &xonxoff);
                printf("%-12s %-16s %7i %i%c%i %s %s\n", name!=NULL?name:undefined, dev, baudrate, databits, paritymode_table[parity], stopbits, ctsrts?"CTS/RTS":"(no hw)", xonxoff?"XONXOFF":"(no sw)");
            }
        } else {
            uarttool_usage(argv[0]);
        }
    }
    mraa_deinit();

    return EXIT_SUCCESS;
}

