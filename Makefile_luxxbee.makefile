# Makefile xbee_lux tab


xbee_out01: xbee_light_test01.o luxsensor.o xbee_uart.o xbee.o
	gcc xbee_light_test01.o luxsensor.o xbee_uart.o xbee.o -o xbee_out01

xbee_light_test01.o : xbee_light_test01.c
	gcc -c xbee_light_test01.c -o xbee_light_test01.o

luxsensor.o: luxsensor.c
	gcc -c luxsensor.c -o luxsensor.o -lwiringPi

xbee_uart.o: xbee_uart.c
	gcc -c xbee_uart.c -o xbee_uart.o

xbee.o : xbee.c
	gcc -I ./xbee_library_files xbee.c -o xbee.o

xbee_light_test01.o : luxsensor.h xbee_uart.h
luxsensor.o : luxsensor.h xbee_uart.h wiringPi.h
xbee_uart.o : xbee_uart.h xbee.h
xbee.o: xbee.h
