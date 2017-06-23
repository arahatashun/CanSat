#ifndef _XBEE_AT
#define _XBEE_AT

#include <inttypes.h>

#ifndef XBEE_PORTNAME
#define XBEE_PORTNAME "/dev/ttyUSB0"
#endif

void xbee_init(void);
void xbeePrintf (const char *message, ...);
void xbee_readln(char *, int);
void xbee_close(void);

#endif
