#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>

#include "mraa/uart.h"

const char* dev_uart="/dev/ttymxc1";
volatile sig_atomic_t loop=1;

void sig_handler(int signum)
{
	if(signum==SIGINT)
	{
		printf("Interrupt Occured Exiting.....\n");
		loop=0;
	}

}

int main(int argc,char** argv)
{
	char buffer[100];
	mraa_uart_context uart;
	mraa_result_t status = MRAA_SUCCESS;
	int baudrate =9600,databits=8,stopbits=1;
	mraa_uart_parity_t parity = MRAA_UART_PARITY_NONE;
	unsigned int ctsrts = 0, xonxoff = 0;
	const char* name=NULL;

	signal(SIGINT,sig_handler);

	mraa_init();

	uart=mraa_uart_init_raw(dev_uart);
	if(uart==NULL)
	{
		fprintf(stderr,"Failed to initilized uart:%s\n",dev_uart);
		return EXIT_FAILURE;
	}

	status=mraa_uart_settings(-1,&dev_uart,&name,&baudrate,&databits,&stopbits,&parity,&ctsrts,&xonxoff);
	if(status!=MRAA_SUCCESS)
	{
		goto error_exit;
	}

	while(loop)
	{
		printf("Enter Message:");
		scanf("%s",buffer);
		mraa_uart_write(uart,buffer,sizeof(buffer));
		sleep(1);
	}

	mraa_uart_stop(uart);
	mraa_deinit();
	return EXIT_SUCCESS;
error_exit:
	mraa_result_print(status);
	mraa_uart_stop(uart);
	mraa_deinit();

	return EXIT_FAILURE;

}
