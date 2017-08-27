#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "xbee_at.h"

//NOTE CoodinatorがPC
//NOTE Routerがraspberry pi
static int usb_filestream = -1;

static void usb_init(void)
{
	usb_filestream = open(XBEE_PORTNAME, O_RDWR | O_NOCTTY | O_NDELAY);

	if (usb_filestream == -1)
	{
		printf("xbee setup error\n");//TODO error handling...
	}
}

static void usb_config(void)
{
	struct termios options;
	tcgetattr(usb_filestream, &options);
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(usb_filestream, TCIFLUSH);
	tcsetattr(usb_filestream, TCSANOW, &options);
}

void xbee_init(void)
{
	usb_init();
	usb_config();
}

static void usbPuts (const char *s)
{
	write (usb_filestream, s, strlen (s));
}

//formatを追加
void xbeePrintf (const char *message, ...)
{
	va_list argp;
	char buffer [1024];

	va_start (argp, message);
	vsnprintf (buffer, 1023, message, argp);
	va_end (argp);
	//printf("%s\n",buffer);
	usbPuts(buffer);
}

// Read a line from USB.
// Return a 0 len string in case of problems with USB
void xbee_readln(char *buffer, int len)
{
	char c;
	char *b = buffer;
	int rx_length = -1;
	while(1) {
		rx_length = read(usb_filestream, (void*)(&c), 1);

		if (rx_length <= 0)
		{
			//wait for messages
			sleep(1);
		} else {
			if (c == '\n') {
				*b++ = '\0';
				break;
			}
			*b++ = c;
		}
	}
}

void xbee_close(void)
{
	close(usb_filestream);
}
