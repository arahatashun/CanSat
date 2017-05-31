#ifndef _XBEE_AT
#define _XBEE_AT

#include <inttypes.h>

#ifndef XBEE_PORTNAME
#define XBEE_PORTNAME "/dev/ttyUSB0"
#endif

void usb_init(void);
void usb_config(void);
void usb_println(const char *, int);
void usb_readln(char *, int);
void usb_close(void);

#endif
