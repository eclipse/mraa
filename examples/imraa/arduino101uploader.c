/**
 * Created by longwei on 03/03/16.
 * Derived from intel-arduino-tools arduino101load go script
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define BAUDRATE B1200

int reset_ping(const char* modem)
{
    struct termios options;
    int uart_fd;
    options.c_cflag |= (CLOCAL | CREAD);
    cfsetospeed(&options,BAUDRATE);
    cfsetispeed(&options,BAUDRATE);
    if((uart_fd = open(modem , O_RDWR | O_NONBLOCK)) == -1){
        printf("Error while opening uart %s \n", modem);
        return -1;
    }
    tcsetattr(uart_fd,TCSANOW,&options);
    close(uart_fd);
    return EXIT_SUCCESS;
}


int main(int argc,char *argv[]) {
    printf("starting download script\n");
    // ARG 1: Path to binaries of dfu, ".../intel-arduino-tools-linux64/x86/bin"
    // ARG 2: Sketch File to download, ".../test.ino.bin"
    // ARG 3: TTY port to use. "/dev/ttyACM0"
    // ARG 4: quiet/verbose
    const char* bin_path = argv[1];
    const char* bin_file_name = argv[2];
    const char* com_port = argv[3];
    bool verbosity = (strncmp(argv[4], "verbose", 5) == 0);

    if (verbosity) {
        printf("bin path: %s\n", bin_path);
        printf("bin img file: %s\n", bin_file_name);
        printf("serial port: %s\n", com_port);
    }

    size_t bin_path_len = strlen(bin_path);
    const char* dfu_upload = "/dfu-util -d,8087:0ABA -D ";
    const char* dfu_option = " -v -a 7 -R";
    char* full_dfu_upload = (char*) malloc (bin_path_len
        + strlen(dfu_upload)+ strlen(dfu_option) + strlen(bin_file_name) + 1);
    strncat(full_dfu_upload, bin_path, strlen(bin_path));
    strncat(full_dfu_upload, dfu_upload, strlen(dfu_upload));
    strncat(full_dfu_upload, bin_file_name, strlen(bin_file_name));
    strncat(full_dfu_upload, dfu_option, strlen(dfu_option));
    if (verbosity) {
        printf("%s\n",full_dfu_upload);
    }
    printf("Forcing reset using 1200bps open/close on port %s\n", com_port);
    reset_ping(com_port);
    sleep(1);
    int status = system(full_dfu_upload);
    free(full_dfu_upload);
    if (status != 0) {
        printf("ERROR: Upload failed on %s\n", com_port);
        exit(1);
    }
    printf("SUCCESS: Sketch will execute in about 5 seconds.\n");
    sleep(5);
    return 0;
}
