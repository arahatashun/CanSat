#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "serial.h"

static int uart0_filestream = -1;

void serial_init(void)
{
	uart0_filestream = open(PORTNAME, O_RDWR | O_NOCTTY | O_NDELAY);

	if (uart0_filestream == -1)
	{
		printf("GPS OPEN FAIL\n");
	}
}

void serial_config(void)
{
	struct termios options;
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);
}

/*
 * serialFlush:
 *	Flush the serial buffers (both tx & rx)
 *********************************************************************************
 */

void serial_flush (void)
{
	printf("serial fush\n");
	tcflush (uart0_filestream, TCIFLUSH);
}

void serial_println(const char *line, int len)
{
	if (uart0_filestream != -1)
	{
		char *cpstr = (char *)malloc((len+1) * sizeof(char));
		strcpy(cpstr, line);
		cpstr[len-1] = '\r';
		cpstr[len] = '\n';

		int count = write(uart0_filestream, cpstr, len+1);
		if (count < 0) {
			//TODO: handle errors...
		}
		free(cpstr);
	}
}

// Read a line from UART.
// Return a 0 len string in case of problems with UART
int serial_readln(char *buffer, int len)
{
	char c;
	char *b = buffer;
	int rx_length = -1;
	int i =0;//flag for timeout
	while(1)
	{
		rx_length = read(uart0_filestream, (void*)(&c), 1);
		if(i==5)
		{
			printf("GPS CONNECTION TIMEOUT\n");
			return -1;
		}
		if (rx_length <= 0)
		{
			printf("%dth read fail\n",i);
			//wait for messages
			sleep(1);
			i++;
		}
		else
		{
			i = 0;
			if (c == '\n')
			{
				*b++ = '\0';
				return 0;
			}
			*b++ = c;
		}
	}
}

int serial_data_avail (void)
{
	int result;
	if (ioctl (uart0_filestream, FIONREAD, &result) == -1)
	{
		printf("WARNING!! SERIAL AVAIL FAIL\n");
		return -1;
	}
	return result;
}


void serial_close(void)
{
	if(close(uart0_filestream)== -1)
	{
		printf("uart0_filestream close failed\n");
		printf("errno=%d: %s\n", errno, strerror(errno));
	}
}
