# Makefile xbee_lux tab
.PHONY:clean

xbeelux_test01: xbee_light_test01.o luxsensor.o xbee_uart.o xbee.o
	gcc xbee_light_test01.o luxsensor.o xbee_uart.o xbee.o -o xbeelux_test01

xbeelux_test01.o : xbee_light_test01.c
	gcc -c xbee_light_test01.c -o xbeelux_test01.o

luxsensor.o: luxsensor.c
	gcc -c luxsensor.c -o luxsensor.o -lwiringPi

xbee_uart.o: xbee_uart.c
	gcc -c xbee_uart.c -o xbee_uart.o

xbee.o : xbee.c
	gcc -c xbee.c -o xbee.o

xbee_light_test01.o : luxsensor.h xbee_uart.h
luxsensor.o : luxsensor.h xbee_uart.h 
xbee_uart.o : xbee_uart.h xbee.h
xbee.o: xbee.h

clean:
	rm -f xbee.o xbee_uart.o luxsensor.o xbeelux_test01.o
