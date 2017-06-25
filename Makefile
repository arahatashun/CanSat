# Makefile
LIBS = -lwiringPi -lm -lgps
objs_g = motor.o mitibiki.o pid.o ring_buffer.o #GPSのみの制御
objs_g_and_c = motor.o mitibiki.o pid.o ring_buffer.o #GPSとコンパス
objs_f = flight_integration.o gut.o luxsensor.o ring_buffer.o #flight

integration.out: $(objs_g)
	gcc -g -Wall -O2 -o integration.out integration.o $(objs_g) $(LIBS)

integration_knd3.out: $(objs_g_and_c)
	gcc -g -Wall -O2 -o integration_knd3.out integration_knd3.o $(objs_g_and_c) $(LIBS)

flight.out: $(objs_f)
	gcc -g -Wall -O2 -o flight_integration.out $(objs_f) $(LIBS)

integration_knd3.o: integration_knd3.c
	gcc -c -Wall integration_knd3.c

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
