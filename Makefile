# Makefile

LIBS =-lwiringPi -lm -lgps
objs = integration.o motor.o

integration.out: $(objs)
	gcc -g -Wall -O2 -o integration.out $(objs) $(LIBS)
integration.o: integration.c
	gcc -c integration.c
motor.o: motor.c
	gcc -c motor.c
