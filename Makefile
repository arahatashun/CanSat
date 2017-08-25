# Makefile
LIBS = -lwiringPi -lm -lgps
objs_g = integration.o  motor.o mitibiki.o pid.o ring_buffer.o #GPSのみの制御
objs_g_and_c = ground_compass.o motor.o mitibiki.o pid.o ring_buffer.o compass.o acclgyro.o#GPSとコンパス
objs_f = flight.o gut.o luxsensor.o ring_buffer.o xbee_at.o acclgyro.o bme280.o #flight


integration.out: $(objs_g)
	gcc -g -Wall -O2 -o integration.out $(objs_g) $(LIBS)

ground_compass.out: $(objs_g_and_c)
	gcc -g -Wall -O2 -o ground_compass.out $(objs_g_and_c) $(LIBS)

flight.out: $(objs_f)
	gcc -g -Wall -O2 -o flight.out $(objs_f) $(LIBS)

sigintHandle.out: sigintHandle.o motor.o gut.o
	gcc -g -Wall -O2 -o sigintHandle.out sigintHandle.o motor.o gut.o -lwiringPi

forward.out: forward.c motor.o
	gcc -g -Wall -O2 -o forward.out forward.c motor.o -lwiringPi

sigintHandle.o: sigintHandle.c
	gcc -c -Wall sigintHandle.c

ground_compass.o: ground_compass.c
	gcc -c -Wall ground_compass.c

flight.o: flight.c
	gcc -c -Wall flight.c

integration.o: integration.c
	gcc -c -Wall integration.c

mitibiki.o: mitibiki.c
	gcc -c -Wall mitibiki.c

motor.o: motor.c
	gcc -c -Wall motor.c

pid.o: pid.c
	gcc -c -Wall pid.c

ring_buffer.o: ring_buffer.c
	gcc -c -Wall ring_buffer.c

gut.o: gut.c
	gcc -c -Wall gut.c

compass.o: compass.c
	gcc -c -Wall compass.c

acclgyro.o: acclgyro.c
	gcc -c -Wall acclgyro.c

xbee_at.o: xbee_at.h xbee_at.c
	gcc -c -Wall xbee_at.c

bme280.o:bme280.h bme280.c
	gcc -c -Wall ../bme280.c
