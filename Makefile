# Makefile

LIBS =-lwiringPi -lm -lgps
objs0 = integration.o motor.o mitibiki.o
objs1 = integration_knd.o motor.o mitibiki.o compass.o acclgyro.o
integration.out: $(objs0)
	gcc -g -Wall -O2 -o integration.out $(objs0) $(LIBS)
knd.out: $(objs1)
	gcc -g -Wall -o knd.out $(objs1) $(LIBS)
integration.o: integration.c
	gcc -c integration.c
integration_knd.o: integration.c
	gcc -c integration_knd.c
mitibiki.o: mitibiki.c
	gcc -c mitibiki.c
compass.o: compass.c
	gcc -c compass.c
acclgyro.o: acclgyro.c
	gcc -c acclgyro.c
motor.o: motor.c
	gcc -c motor.c
