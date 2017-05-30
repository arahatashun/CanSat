#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "xbee_at.h"

int usb_filestream = -1;

void usb_init(void)
{
    usb_filestream = open(XBEE_PORTNAME, O_RDWR | O_NOCTTY | O_NDELAY);

    if (usb_filestream == -1)
    {
        printf("error");//TODO error handling...
    }
}

void usb_config(void)
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

void usb_println(const char *line, int len)
{
    if (usb_filestream != -1) {
        char *cpstr = (char *)malloc((len+1) * sizeof(char));
        strcpy(cpstr, line);
        cpstr[len-1] = '\r';
        cpstr[len] = '\n';

        int count = write(usb_filestream, cpstr, len+1);
        if (count < 0) {
            printf("errno=%d: %s\n",errno, strerror(errno));
            printf("print error\n");//TODO: handle errors...
        }
        free(cpstr);
    }
}

// Read a line from USB.
// Return a 0 len string in case of problems with USB
void usb_readln(char *buffer, int len)
{
    char c;
    char *b = buffer;
    int rx_length = -1;
    while(1) {
        rx_length = read(usb_filestream, (void*)(&c), 1);

        if (rx_length <= 0) {
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

void usb_close(void)
{
    close(usb_filestream);
}

int main()
{
  usb_init();
  usb_config();
  while (1)
  {
  char buffer[7] = "hoge";
  usb_println(buffer, 7);
  sleep(3);
  }
}
