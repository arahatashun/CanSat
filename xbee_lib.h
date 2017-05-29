//xbee ライブラリヘッダファイル xbee_lib.h

#ifndef XBEELIB_H
#define XBEELIB_H

typedef unsigned char byte;

byte xbee_init(const byte port);
void xbee_from(byte *address);
byte xbee_atnj(const byte timeout);
byte xbee_ratnj(const byte *address, const byte timeout);
byte xbee_uart_char(const byte *address, const char *in);
void xbee_uart_int(const byte *address, int in);
void xbee_uart_double(const byte *address, double in);


#endif
