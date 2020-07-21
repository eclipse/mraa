
/*
 * Author: Dan O'Donovan <dan@emutex.com>
 * Author: Santhanakumar A <santhanakumar.a@adlinktech.com>

 * Copyright (c) 2019 ADLINK Technology Inc.
 * SPDX-License-Identifier: MIT

 */


#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <sys/file.h>
#include <unistd.h>

#include "linux/gpio.h"
#include "mraa_internal.h"

#include <dirent.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <linux/i2c-dev.h>


#include "common.h"
#include "gpio.h"
#include "x86/intel_adlink_lec_al.h"
#include "gpio/gpio_chardev.h"

#define SYSFS_CLASS_GPIO "/sys/class/gpio"

#define PLATFORM_NAME "LEC-AL"
#define PLATFORM_NAME_AI "LEC-AL AI"

#define MRAA_LEC_AL_GPIOCOUNT  17
#define MAX_SIZE 64
#define POLL_TIMEOUT

static volatile int base1, base2, _fd;
static volatile long m_period;
static mraa_gpio_context gpio;


struct intr_list {
	volatile int pin;
	unsigned char curr;
	char valuepath[50];
	void (*fptr)(void*);
	void* args;
	struct intr_list *next;
};

static unsigned char regIon[16]   = {0x2A, 0x2D, 0x30, 0x33, 0x36, 0x3B, 0x40, 0x45, 0x4A, 0x4D, 0x50, 0x53, 0x56, 0x5B, 0x60, 0x65};

static unsigned int  IonValue[16]   = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static struct intr_list *list;
mraa_result_t gpio_intr_init_pre(int pin);
static int sx150x_pwm_init(int pin);

static mraa_result_t pwm_init_raw_replace(mraa_pwm_context dev, int pin)
{
	char buffer[100] = {0};
	int i, fd;

	if(pin < 9)
	{
		if(sx150x_pwm_init(pin))
		{
			return MRAA_ERROR_NO_RESOURCES;
		}
		if((fd = open("/sys/class/gpio/export", O_WRONLY)) != -1)
		{
			i = sprintf(buffer,"%d",base2 + pin);
			write(fd, buffer, i);
			close(fd);
			sprintf(buffer,"/sys/class/gpio/gpio%d/direction",base2 + pin);
			if((fd = open(buffer, O_WRONLY)) != -1)
			{
				write(fd, "out", 3);
				close(fd);
				sprintf(buffer,"/sys/class/gpio/gpio%d/value",base2 + pin);
				if((fd = open(buffer, O_WRONLY)) != -1)
				{
					write(fd, "0", 1);
					close(fd);
				}
			}
		}
		else
		{
			return MRAA_ERROR_NO_RESOURCES;
		}
	}
	return MRAA_SUCCESS;
}

static mraa_result_t pwm_period_replace(mraa_pwm_context dev, int period)
{
	m_period = period;
	return MRAA_SUCCESS;
}

static float pwm_read_replace(mraa_pwm_context dev)
{
	unsigned char rx_tx_buf[3] = {0};

	if(dev->pin < 9)
	{
		if(_fd == -1)
		{
			return 0;
		}

		rx_tx_buf[0] = regIon[dev->pin];
		if(write(_fd, &(rx_tx_buf[0]), 1) == 1)
		{
			if(read(_fd, &(rx_tx_buf[1]), 1) == 1)
			{
				return (rx_tx_buf[1] / 2.55);
			}
		}
	}
	return 0;
}


static mraa_result_t pwm_write_replace(mraa_pwm_context dev, float duty)
{
	unsigned char rx_tx_buf[3] = {0};

	IonValue[dev->pin] = ((duty /m_period) * 255);

	if(dev->pin < 9)
	{
		if(_fd == -1)
		{
			return MRAA_ERROR_INVALID_RESOURCE;
		}

		rx_tx_buf[0] = regIon[dev->pin];
		rx_tx_buf[1] = IonValue[dev->pin];

		if(write(_fd, &(rx_tx_buf[0]), 2) != 2)
		{
			return MRAA_ERROR_NO_RESOURCES;
		}
		return MRAA_SUCCESS;
	}
	return MRAA_ERROR_NO_RESOURCES;
}

static mraa_result_t pwm_enable_replace(mraa_pwm_context dev, int enable)
{
	int pin = dev->pin;

	if(9 > pin && 0 <= pin)
	{
		if(_fd != -1)
		{
			unsigned char rx_tx_buf[3] = {0};

			if(_fd == -1)
			{
				return MRAA_ERROR_NO_RESOURCES;
			}

			rx_tx_buf[0] = regIon[pin];
			rx_tx_buf[1] = 0xFF;
			if(write(_fd, &(rx_tx_buf[0]), 2) != 2)
			{
				return MRAA_ERROR_NO_RESOURCES;
			}
		}
		else
		{
			return MRAA_ERROR_NO_RESOURCES;
		}

		return MRAA_SUCCESS;
	}
	return MRAA_ERROR_NO_RESOURCES;
}

	static void
gpio_close_event_handles_sysfs(int fds[], int num_fds)
{
	if ((fds == NULL) || (num_fds <= 0)) {
		syslog(LOG_CRIT, "failed to close and free sysfs event handles");
		return;
	}

	for (int i = 0; i < num_fds; ++i) {
		close(fds[i]);
	}

	free(fds);
}


	static mraa_timestamp_t
gpio_get_timestamp_sysfs()
{
	struct timeval time;
	gettimeofday(&time, NULL);

	return (time.tv_sec * 1e6 + time.tv_usec);
}

//clearing all interrupts
void clear_sx1509x_intr(void)
{
	uint8_t rx_tx_buf[10] = {0};

	rx_tx_buf[0] = 0x18;
	rx_tx_buf[1] = 0xFF;
	rx_tx_buf[2] = 0xFF;
	write(_fd, rx_tx_buf, 3);
}


static mraa_result_t gpio_wait_interrupt(int fds[], int num_fds, mraa_gpio_events_t events)
{
	unsigned char c;
	struct pollfd pfd[num_fds];

	pfd[0].fd = fds[0];
	// setup poll on POLLPRI
	pfd[0].events = POLLPRI;

	// do an initial read to clear interrupt
	lseek(fds[0], 0, SEEK_SET);
	read(fds[0], &c, 1);

	clear_sx1509x_intr();

	// Wait for it forever or until pthread_cancel
	// poll is a cancelable point like sleep()
	poll(pfd, num_fds, -1);

	clear_sx1509x_intr();

	if (pfd[0].revents & POLLPRI) {
		read(fds[0], &c, 1);
		events[0].id = 0;
		events[0].timestamp = gpio_get_timestamp_sysfs();
	} else
		events[0].id = -1;

	return MRAA_SUCCESS;
}

	static void*
gpio_interrupt_handler(void* arg)
{
	if (arg == NULL) {
		syslog(LOG_ERR, "gpio: interrupt_handler: context is invalid");
		return NULL;
	}

	mraa_result_t ret;
	mraa_gpio_context dev = (mraa_gpio_context) arg;
	int idx = 0;

	int *fps = malloc(dev->num_pins * sizeof(int));
	if (!fps) {
		syslog(LOG_ERR, "mraa_gpio_interrupt_handler_multiple() malloc error");
		return NULL;
	}

	mraa_gpio_context it = dev;

	char bu[MAX_SIZE];
	snprintf(bu, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/value", it->pin);
	fps[idx] = open(bu, O_RDONLY);
	if (fps[idx] < 0) {
		syslog(LOG_ERR, "gpio%i: interrupt_handler: failed to open 'value' : %s", it->pin,
				strerror(errno));
		gpio_close_event_handles_sysfs(fps, idx);
		return NULL;
	}

	idx++;

	for (;;) {
		ret = gpio_wait_interrupt(fps, idx, dev->events);

		if (ret == MRAA_SUCCESS && !dev->isr_thread_terminating) {
			pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
			if(dev->isr != NULL)
			{
				(dev->isr)(dev->isr_args);
			}
			pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		}
	}
}

static void internal_isr(void*args)
{
	struct intr_list *it = list;
	int fps;
	unsigned char c;

	/* Is this pin on a subplatform? Do nothing... */
	while (it) {
		// open gpio value with open(3)
		fps = open(it->valuepath, O_RDONLY);
		if (fps > 0) {
			read(fps, &c, 1);
			close(fps);
			if(it->curr != c)
			{
				(it->fptr)(it->args);
				it->curr = c;
			}

		}
		it = it->next;
	}
}

// configuring main interrupt line for sx1509q it will create common isr routine for all gpio's
static mraa_result_t intr_init()
{
	int fd;
	char bu[MAX_SIZE];
	int length;
	char filepath[MAX_SIZE];

	gpio = (mraa_gpio_context) calloc(1, sizeof(struct _gpio));
	if (gpio == NULL) {
		syslog(LOG_CRIT, "gpio%i: Failed to allocate memory for context", 456);
		return MRAA_ERROR_INVALID_RESOURCE;
	}

	gpio->advance_func = NULL;
	gpio->pin = 434 + 22;
	gpio->value_fp = -1;
	gpio->isr_value_fp = -1;

	gpio->isr_thread_terminating = 0;
	gpio->owner = 1;

	gpio->num_pins = 1;
	gpio->next = NULL;
	gpio->events = NULL;

	if((fd = open("/sys/class/gpio/export", O_WRONLY)) != -1)
	{
		write(fd,"456",3);
		sprintf(bu,"%d",base1 + 12);
		write(fd,bu,3);
		close(fd);
	}
	else
	{
		return MRAA_ERROR_INVALID_RESOURCE;
	}

	sprintf(bu,"/sys/class/gpio/gpio%d/direction",456);
	if((fd = open(bu, O_WRONLY)) != -1)
	{
		write(fd, "in", 2);
		close(fd);
	}
	else
	{
		return MRAA_ERROR_INVALID_RESOURCE;
	}

	sprintf(bu,"/sys/class/gpio/gpio%d/direction",base1 + 12);
	if((fd = open(bu, O_WRONLY)) != -1)
	{
		write(fd, "in", 2);
		close(fd);
	}
	else
	{
		return MRAA_ERROR_INVALID_RESOURCE;
	}

	// we only allow one isr per mraa_gpio_context
	if (gpio->thread_id != 0) {
		return MRAA_ERROR_NO_RESOURCES;
	}

	gpio->events = malloc(gpio->num_pins * sizeof(mraa_gpio_event));
	if (gpio->events == NULL) {
		syslog(LOG_ERR, "mraa_gpio_edge_mode() malloc error");
		return MRAA_ERROR_NO_RESOURCES;
	}

	gpio->events[0].id = -1;

	snprintf(filepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/edge", gpio->pin);

	int edge = open(filepath, O_RDWR);
	if (edge == -1) {
		syslog(LOG_ERR, "gpio%i: edge_mode: Failed to open 'edge' for writing: %s", gpio->pin,
				strerror(errno));
		return MRAA_ERROR_INVALID_RESOURCE;
	}

	length = snprintf(bu, sizeof(bu), "both");
	if (write(edge, bu, length * sizeof(char)) == -1) {
		syslog(LOG_ERR, "gpio%i: edge_mode: Failed to write to 'edge': %s", gpio->pin, strerror(errno));
		close(edge);
		return MRAA_ERROR_UNSPECIFIED;
	}

	close(edge);

	gpio->isr = internal_isr;
	gpio->isr_args = NULL;

	pthread_create(&gpio->thread_id, NULL, gpio_interrupt_handler, (void*) gpio);

	return MRAA_SUCCESS;
}

// it will clean the resources, MRAA created by advanced callback functions
static mraa_result_t gpio_close_pre(mraa_gpio_context dev)
{
	struct intr_list *ptr, *last;
	ptr = last = list;
	char gpio_path[50] = {0};
	int fd, length;

	if(list == NULL)
	{
		return MRAA_SUCCESS;
	}

	while(ptr)
	{
		if(ptr->pin == dev->pin)
		{
			break;
		}
		last = ptr;
		ptr = ptr->next;
	}

	if(ptr)
	{
		if(ptr == list)
		{
			list = ptr->next;
		}
		else
		{
			last->next = ptr->next;
		}
	}


	if(list == NULL && gpio != NULL)
	{
		mraa_gpio_isr_exit(gpio);
		if((fd = open("/sys/class/gpio/unexport", O_WRONLY)) != -1)
		{
			length = sprintf(gpio_path,"%d",gpio->pin);
			write(fd, gpio_path, length);
			length = sprintf(gpio_path,"%d",base1 + 12);
			write(fd, gpio_path, length);
			close(fd);
		}

		free(gpio);
		gpio = NULL;
	}

	return MRAA_SUCCESS;
}

//callb ack routine for isr registration
static mraa_result_t gpio_isr_replace(mraa_gpio_context dev, mraa_gpio_edge_t mode, void (*fptr)(void*), void* args)
{
	struct intr_list *node = list;
	int fps;

	node = list;

	if(list == NULL)
	{
		intr_init();
	}

	gpio_intr_init_pre(dev->pin);

	struct intr_list *ptr = list;

	while(ptr)
	{
		if(ptr->pin == dev->pin)
		{
			return MRAA_ERROR_NO_RESOURCES;
		}
		ptr = ptr->next;
	}

	list = (struct intr_list*)calloc(1, sizeof(struct intr_list));
	if(list == NULL)
	{
		return MRAA_ERROR_INVALID_RESOURCE;
	} 
	list->fptr = fptr;
	list->args = args;
	list->pin = dev->pin;
	list->next = node;

	snprintf(list->valuepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/value", list->pin);
	fps = open(list->valuepath, O_RDONLY);
	if (fps > 0) {
		read(fps, &(list->curr), 1);
		close(fps);
	}

	return MRAA_SUCCESS;
}


//configuring extra registers as pwm for extended gpio
static int sx150x_pwm_init(int pin)
{
	int i;
	int add = (pin < 8) ? 1 : 0;

	unsigned char rx_tx_buf[] = {0x0 + add, 0, 0x6 + add, 0, 0xE + add, 0, 0x20 + add, 0, 0x10 + add, 0};

	if(_fd == -1)
	{
		return -1;
	}


	for(i = 0; i < 9; i += 2)
	{
		if(write(_fd, &(rx_tx_buf[i]), 1) != 1)
		{
			return -1;
		}
		if(read(_fd, &(rx_tx_buf[i + 1]), 1) != 1)
		{
			return -1;
		}

		if((i == 2) || (i == 6))
			rx_tx_buf[i+1] |= (1 << (pin % 8));
		else
			rx_tx_buf[i+1]  &= ~(1 << (pin % 8));

		if(write(_fd, &(rx_tx_buf[i]), 2) != 2)
		{
			return -1;
		}
	}

	return 0;
}

//configuring extra registers as interrupt for extended gpio
mraa_result_t gpio_intr_init_pre(int pin)
{
	int i;
	if(base2 + 17 > pin && (base2 - 1) < pin)
	{
		pin = (pin - base2);
		int index = pin % 8;

		unsigned char rx_tx_buf[] = {0x12, 0, 0, 0x14, 0, 0, 0x16, 0, 0, 0x18, 0, 0};

		if(_fd == -1)
		{
			return MRAA_ERROR_NO_RESOURCES;
		}

		for(i = 0; i < 10; i+=3)
		{
			if(write(_fd, &(rx_tx_buf[i]), 1) != 1)
			{
				return MRAA_ERROR_NO_RESOURCES;
			}
			if(read(_fd, &(rx_tx_buf[i + 1]), 2) != 2)
			{
				return MRAA_ERROR_NO_RESOURCES;
			}


			if(rx_tx_buf[i] == 0x12)
			{
				if(pin < 8)
					rx_tx_buf[i + 2] &= ~(1 << index);
				else
					rx_tx_buf[i + 1] &= ~(1 << index);
			}
			else 
			{
				if(rx_tx_buf[i] == 0x14 && pin < 8)
				{
					if(index < 4)
						rx_tx_buf[i + 2] |= (3 << ((index % 4) * 2));
					else
						rx_tx_buf[i + 1] |= (3 << ((index % 4) * 2));
				}
				else if(rx_tx_buf[i] == 0x16)
				{
					if(index < 4)
						rx_tx_buf[i + 2] |= (3 << ((index % 4) * 2));
					else
						rx_tx_buf[i + 1] |= (3 << ((index % 4) * 2));
				}
				else
				{
					if(pin < 8)
						rx_tx_buf[i + 2] |= (1 << index);
					else
						rx_tx_buf[i + 1] |= (1 << index);
				}
			}

			if(write(_fd, &(rx_tx_buf[i]), 3) != 3)
			{
				return MRAA_ERROR_NO_RESOURCES;
			}
		}
	}
	return MRAA_SUCCESS;
}

static int sx150x_init(int bus_num)
{
	char rx_tx_buf[20] = {0};

	sprintf(rx_tx_buf, "/dev/i2c-%d",bus_num);
	if((_fd = open(rx_tx_buf, O_RDWR)) < 0)
	{
		return -1;
	}

	if(ioctl(_fd, I2C_SLAVE_FORCE, 0x3E) < 0)
	{
		return -1;
	}
	/*
	// resetting sx1509q
	rx_tx_buf[0] = 0x7d;
	rx_tx_buf[1] = 0x12;

	if(write(_fd, &(rx_tx_buf[0]), 2) == 2)
	{
	rx_tx_buf[1] = 0x34;
	if(write(_fd, &(rx_tx_buf[0]), 2) != 2)
	{
	return -1;
	}
	}
	 */
	// configuring clock and misc register for PWM feature
	rx_tx_buf[0] = 0x1E;
	if(write(_fd, &(rx_tx_buf[0]), 1) == 1)
	{	
		if(read(_fd, &(rx_tx_buf[1]), 1) == 1)
		{
			rx_tx_buf[1] |= (1 << 6);
			rx_tx_buf[1] |= ~(1 << 5);
			if(write(_fd, &(rx_tx_buf[0]), 2) != 2)
			{
				return -1;
			}
		}
	}
	else
	{
		return -1;
	}

	rx_tx_buf[0] = 0x1F;
	if(write(_fd, &(rx_tx_buf[0]), 1) == 1)
	{	
		if(read(_fd, &(rx_tx_buf[1]), 1) == 1)
		{
			rx_tx_buf[1] &= ~(1 << 7);
			rx_tx_buf[1] &= ~(1 << 3);
			rx_tx_buf[1] &= ~((0x7) << 4);
			rx_tx_buf[1] |= ((1 & 0x7) << 4);
			if(write(_fd, &(rx_tx_buf[0]), 2) == 2)
			{
				return 0;
			}
		}
	}

	return -1;
}


// utility function to setup pin mapping of boards
static mraa_result_t mraa_lec_al_set_pininfo(mraa_board_t* board, int mraa_index, char* name,
		mraa_pincapabilities_t caps, int sysfs_pin)
{
	if (mraa_index < board->phy_pin_count) {
		mraa_pininfo_t* pin_info = &board->pins[mraa_index];
		strncpy(pin_info->name, name, MRAA_PIN_NAME_SIZE);
		pin_info->capabilities = caps;
		if (caps.gpio) {
			pin_info->gpio.pinmap = sysfs_pin;
			pin_info->gpio.mux_total = 0;
		}
		if (caps.i2c) {
			pin_info->i2c.pinmap = 1;
			pin_info->i2c.mux_total = 0;
		}

		if (caps.uart) {
			pin_info->uart.mux_total = 0;
		}
		if (caps.spi) {
			pin_info->spi.mux_total = 0;
		}
		return MRAA_SUCCESS;
	}
	return MRAA_ERROR_INVALID_RESOURCE;
}

static mraa_result_t mraa_lec_al_get_pin_index(mraa_board_t* board, char* name, int* pin_index)
{
	int i;
	for (i = 0; i < board->phy_pin_count; ++i) {
		if (strncmp(name, board->pins[i].name, MRAA_PIN_NAME_SIZE) == 0) {
			*pin_index = i;
			return MRAA_SUCCESS;
		}
	}

	syslog(LOG_CRIT, "lec_al: Failed to find pin name %s", name);

	return MRAA_ERROR_INVALID_RESOURCE;
}

static mraa_result_t gpio_init_pre(int pin)
{
	unsigned char rx_tx_buf[3] = {0};
	int fd, i;
	char buffer[50] = {0};

	if(pin == base1 + 6)
	{
		for(i = 0; i < 100; i++)
		{
			sprintf(buffer, "/sys/class/hwmon/hwmon%d/device/fan1_enable", i);

			if((fd = open(buffer, O_RDWR)) == -1)
			{
				continue;
			}
			if((write(fd, "1", 1)) != 1)
			{
				close(fd);
				return MRAA_ERROR_NO_RESOURCES;
			}
			close(fd);

		}
	}

	if(base2 + 17 > pin && (base2 - 1) < pin)
	{
		pin = pin - base2;

		int add = (pin < 8) ? 1 : 0;

		if(_fd == -1)
		{
			return MRAA_ERROR_INVALID_RESOURCE;
		}

		rx_tx_buf[0] = regIon[pin];
		rx_tx_buf[1] = -1;

		if(write(_fd, &(rx_tx_buf[0]), 2) != 2)
		{
			return MRAA_ERROR_NO_RESOURCES;
		}

		rx_tx_buf[0] = 0x0 + add;
		if(write(_fd, &(rx_tx_buf[0]), 1) == 1)
		{
			if(read(_fd, &(rx_tx_buf[1]), 1) == 1)
			{
				rx_tx_buf[1] &= ~(1 < (pin % 8));
				write(_fd, &rx_tx_buf[0], 2);
			}
		}

		rx_tx_buf[0] = 0x20 + add;
		if(write(_fd, &(rx_tx_buf[0]), 1) == 1)
		{
			if(read(_fd, &(rx_tx_buf[1]), 1) == 1)
			{
				rx_tx_buf[1] &= ~(1 < (pin % 8));
				write(_fd, &rx_tx_buf[0], 2);
				if((fd = open("/sys/class/gpio/unexport", O_WRONLY)) != -1)
				{
					i = sprintf(buffer,"%d",base2 + pin);
					write(fd, buffer, i);
					close(fd);
				}
				return MRAA_SUCCESS;
			}
		}
		return MRAA_ERROR_INVALID_RESOURCE;
	}
	return MRAA_SUCCESS;
}

mraa_board_t* mraa_lec_al_board()
{
	int i, fd, i2c_bus_num;
	int lecalai  = -1;
	char buffer[60] = {0}, *line = NULL;
	FILE *fh;
	size_t len;

	mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof (mraa_board_t));

	if (b == NULL) {
		return NULL;
	}

	if((fh = fopen("/sys/devices/virtual/dmi/id/product_name", "r")) != NULL) {
		if (getline(&line, &len, fh) != -1) {
			line[strcspn(line, "\r\n")] = 0;
			if ((strncasecmp(line, "LEC-AL-AI", strlen("LEC-AL-AI") + 1) == 0)) {
				b->platform_name = PLATFORM_NAME_AI;
				lecalai = 1;
			}
			else
			{
				lecalai = 0;
				b->platform_name = PLATFORM_NAME;
			}
		}
		fclose(fh);
	}

	b->phy_pin_count = MRAA_LEC_AL_PINCOUNT;
	b->gpio_count = MRAA_LEC_AL_GPIOCOUNT;
	b->chardev_capable = 0;

	b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * MRAA_LEC_AL_PINCOUNT);
	if (b->pins == NULL) {
		goto error;
	}

	b->adv_func = (mraa_adv_func_t *) calloc(1, sizeof (mraa_adv_func_t));
	if (b->adv_func == NULL) {
		free(b->pins);
		goto error;
	}

	b->adv_func->gpio_isr_replace = gpio_isr_replace;
	b->adv_func->gpio_close_pre = gpio_close_pre;
	b->adv_func->gpio_init_pre = gpio_init_pre;

	// initializations of pwm functions
	b->adv_func->pwm_init_raw_replace = pwm_init_raw_replace;
	b->adv_func->pwm_period_replace = pwm_period_replace;
	b->adv_func->pwm_read_replace = pwm_read_replace;
	b->adv_func->pwm_write_replace = pwm_write_replace;
	b->adv_func->pwm_enable_replace = pwm_enable_replace;

	for(i = 0; i < 999; i++)
	{
		sprintf(buffer,"/sys/class/gpio/gpiochip%d/device/name",i);
		if((fd = open(buffer, O_RDONLY)) != -1)
		{
			int count = read(fd,buffer,7);
			if(count != 0)
			{
				if(strncmp(buffer, "sx1509q", count) == 0)
				{
					base2 = i;
				}
				if(strncmp(buffer, "pca9535", count) == 0)
				{
					base1 = i;
				}
			}
			close(fd);
		}
	}

	syslog(LOG_NOTICE, "lec_al: base1 %d base2 %d\n", base1, base2);

	// Configure PWM
	b->pwm_dev_count = 0;
	b->pwm_default_period = 5000;
	b->pwm_max_period = 218453;
	b->pwm_min_period = 1;

	mraa_lec_al_set_pininfo(b, 1,  "3v3",        (mraa_pincapabilities_t){ -1, 0, 0, 0, 0, 0, 0, 0 }, -1);
	mraa_lec_al_set_pininfo(b, 2,  "5v",         (mraa_pincapabilities_t){ -1, 0, 0, 0, 0, 0, 0, 0 }, -1);
	mraa_lec_al_set_pininfo(b, 3,  "I2C0_DAT",   (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 }, -1);
	mraa_lec_al_set_pininfo(b, 4,  "5v",         (mraa_pincapabilities_t){ -1, 0, 0, 0, 0, 0, 0, 0 }, -1);
	mraa_lec_al_set_pininfo(b, 5,  "I2C0_CK",    (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 }, -1);
	mraa_lec_al_set_pininfo(b, 6,  "GND",        (mraa_pincapabilities_t){ -1, 0, 0, 0, 0, 0, 0, 0 }, -1);
	mraa_lec_al_set_pininfo(b, 7,  "GPIO04",     (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, base1 + 4);
	mraa_lec_al_set_pininfo(b, 8,  "UART_TXD",   (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 }, -1);
	mraa_lec_al_set_pininfo(b, 9,  "GND",        (mraa_pincapabilities_t){ -1, 0, 0, 0, 0, 0, 0, 0 }, -1);
	mraa_lec_al_set_pininfo(b, 10, "UART_RXD",   (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 }, -1);
	mraa_lec_al_set_pininfo(b, 11, "GPIO05",     (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, base1 + 6);
	mraa_lec_al_set_pininfo(b, 12, "GPIO06",     (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, base1 + 5);
	b->pins[12].pwm.parent_id = 0;
	b->pins[12].pwm.pinmap = 9;
	b->pwm_dev_count++;
	mraa_lec_al_set_pininfo(b, 13, "GPIO07",     (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, base1 + 7);
	mraa_lec_al_set_pininfo(b, 14, "GND",        (mraa_pincapabilities_t){ -1, 0, 0, 0, 0, 0, 0, 0 }, -1);
	mraa_lec_al_set_pininfo(b, 15, "GPIO08",     (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, base1 + 8);
	mraa_lec_al_set_pininfo(b, 16, "GPIO09",     (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, base1 + 9);
	mraa_lec_al_set_pininfo(b, 17, "3v3",        (mraa_pincapabilities_t){ -1, 0, 0, 0, 0, 0, 0, 0 }, -1);
	mraa_lec_al_set_pininfo(b, 18, "GPIO10",     (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, base1 + 10);
	mraa_lec_al_set_pininfo(b, 19, "SPI_0_MOSI", (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 }, -1);
	mraa_lec_al_set_pininfo(b, 20, "GND",        (mraa_pincapabilities_t){ -1, 0, 0, 0, 0, 0, 0, 0 }, -1);
	mraa_lec_al_set_pininfo(b, 21, "SPI_0_MISO", (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 }, -1);
	mraa_lec_al_set_pininfo(b, 22, "GPIO11",     (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, base1 + 11);
	mraa_lec_al_set_pininfo(b, 23, "SPI_0_SCLK", (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 }, -1);
	mraa_lec_al_set_pininfo(b, 24, "SPI_0_CE0",  (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 }, -1);
	mraa_lec_al_set_pininfo(b, 25, "GND",        (mraa_pincapabilities_t){ -1, 0, 0, 0, 0, 0, 0, 0 }, -1);
	mraa_lec_al_set_pininfo(b, 26, "SPI_0_CE1",  (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 }, -1);
	mraa_lec_al_set_pininfo(b, 27, "I2C1_DAT",   (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 }, -1);
	mraa_lec_al_set_pininfo(b, 28, "I2C1_CK",    (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 }, -1);

	mraa_lec_al_set_pininfo(b, 29, "GPIO1_0",    (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 }, base2 + 0);
	b->pins[29].pwm.parent_id = 1;
	b->pins[29].pwm.pinmap = 0;
	b->pwm_dev_count++;
	mraa_lec_al_set_pininfo(b, 30, "GND",        (mraa_pincapabilities_t){ -1, 0, 0, 0, 0, 0, 0, 0 }, -1);
	mraa_lec_al_set_pininfo(b, 31, "GPIO1_1",    (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 }, base2 + 1);
	b->pins[31].pwm.parent_id = 1;
	b->pins[31].pwm.pinmap = 1;
	b->pwm_dev_count++;
	mraa_lec_al_set_pininfo(b, 32, "GPIO1_2",    (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 }, base2 + 2);
	b->pins[32].pwm.parent_id = 1;
	b->pins[32].pwm.pinmap = 2;
	b->pwm_dev_count++;
	mraa_lec_al_set_pininfo(b, 33, "GPIO1_3",    (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 }, base2 + 3);
	b->pins[33].pwm.parent_id = 1;
	b->pins[33].pwm.pinmap = 3;
	b->pwm_dev_count++;
	mraa_lec_al_set_pininfo(b, 34, "GND",        (mraa_pincapabilities_t){ -1, 0, 0, 0, 0, 0, 0, 0 }, -1);
	mraa_lec_al_set_pininfo(b, 35, "GPIO1_4",    (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 }, base2 + 4);
	b->pins[35].pwm.parent_id = 1;
	b->pins[35].pwm.pinmap = 4;
	b->pwm_dev_count++;
	mraa_lec_al_set_pininfo(b, 36, "GPIO1_5",    (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 }, base2 + 5);
	b->pins[36].pwm.parent_id = 1;
	b->pins[36].pwm.pinmap = 5;
	b->pwm_dev_count++;
	mraa_lec_al_set_pininfo(b, 37, "GPIO1_6",    (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 }, base2 + 6);
	b->pins[37].pwm.parent_id = 1;
	b->pins[37].pwm.pinmap = 6;
	b->pwm_dev_count++;
	mraa_lec_al_set_pininfo(b, 38, "GPIO1_7",    (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 }, base2 + 7);
	b->pins[38].pwm.parent_id = 1;
	b->pins[38].pwm.pinmap = 7;
	b->pwm_dev_count++;
	mraa_lec_al_set_pininfo(b, 39, "GND",        (mraa_pincapabilities_t){ -1, 0, 0, 0, 0, 0, 0, 0 }, -1);
	mraa_lec_al_set_pininfo(b, 40, "GPIO2_8",    (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 }, base2 + 8);
	b->pins[40].pwm.parent_id = 1;
	b->pins[40].pwm.pinmap = 8;
	b->pwm_dev_count++;


	// Configure UART #1 (default)
	b->uart_dev_count = 1;

	mraa_lec_al_get_pin_index(b, "UART_RXD",  &(b->uart_dev[0].rx));
	mraa_lec_al_get_pin_index(b, "UART_TXD",  &(b->uart_dev[0].tx));
	b->uart_dev[0].device_path = "/dev/ttyS4";

	b->def_uart_dev = 0;

	// Configure SPI #0 CS1
	b->spi_bus_count = 0;
	b->spi_bus[b->spi_bus_count].bus_id = 1;
	b->spi_bus[b->spi_bus_count].slave_s = 0;
	mraa_lec_al_get_pin_index(b, "SPI_0_CE0",  &(b->spi_bus[b->spi_bus_count].cs));
	mraa_lec_al_get_pin_index(b, "SPI_0_MOSI", &(b->spi_bus[b->spi_bus_count].mosi));
	mraa_lec_al_get_pin_index(b, "SPI_0_MISO", &(b->spi_bus[b->spi_bus_count].miso));
	mraa_lec_al_get_pin_index(b, "SPI_0_SCLK",  &(b->spi_bus[b->spi_bus_count].sclk));
	b->spi_bus_count++;

	b->spi_bus[b->spi_bus_count].bus_id = 1;
	b->spi_bus[b->spi_bus_count].slave_s = 1;
	mraa_lec_al_get_pin_index(b, "SPI_0_CE1",  &(b->spi_bus[b->spi_bus_count].cs));
	mraa_lec_al_get_pin_index(b, "SPI_0_MOSI", &(b->spi_bus[b->spi_bus_count].mosi));
	mraa_lec_al_get_pin_index(b, "SPI_0_MISO", &(b->spi_bus[b->spi_bus_count].miso));
	mraa_lec_al_get_pin_index(b, "SPI_0_SCLK",  &(b->spi_bus[b->spi_bus_count].sclk));
	b->spi_bus_count++;

	// Set number of i2c adaptors usable from userspace
	b->i2c_bus_count = 0;
	b->def_i2c_bus = 0;

	i2c_bus_num = mraa_find_i2c_bus_pci("0000:00", "0000:00:16.1", "i2c_designware.1");
	if (i2c_bus_num != -1) {
		if(sx150x_init(i2c_bus_num) < 0)
		{
			_fd = -1;
		}

		b->i2c_bus[0].bus_id = i2c_bus_num;
		mraa_lec_al_get_pin_index(b, "I2C1_DAT", (int*) &(b->i2c_bus[1].sda));
		mraa_lec_al_get_pin_index(b, "I2C1_CK", (int*) &(b->i2c_bus[1].scl));
		b->i2c_bus_count++;
	}

	if(lecalai == 1)
	{
		i2c_bus_num = mraa_find_i2c_bus_pci("0000:00", "0000:00:16.3", "i2c_designware.3");
	}
	else if(lecalai == 0)
	{
		i2c_bus_num = mraa_find_i2c_bus_pci("0000:00", "0000:00:1f.1", ".");
	}
	if (i2c_bus_num != -1) {
		b->i2c_bus[1].bus_id = i2c_bus_num;
		mraa_lec_al_get_pin_index(b, "I2C0_DAT", (int*) &(b->i2c_bus[0].sda));
		mraa_lec_al_get_pin_index(b, "I2C0_CK", (int*) &(b->i2c_bus[0].scl));
		b->i2c_bus_count++;
	}

	const char* pinctrl_path = "/sys/bus/platform/drivers/broxton-pinctrl";
	int have_pinctrl = access(pinctrl_path, F_OK) != -1;
	syslog(LOG_NOTICE, "lec_al: kernel pinctrl driver %savailable", have_pinctrl ? "" : "un");

	return b;

error:
	syslog(LOG_CRIT, "lec_al: Platform failed to initialise");
	free(b);
	close(_fd);
	return NULL;
}
