# Makefile

LIBS =-lwiringPi -lm -lgps
objs = test.o motor.o compass.o mitibiki.o

a.out: $(objs)
	gcc -Wall -O2 -o a.out $(objs) $(LIBS)
test.o: test.c
	gcc -c test.c
motor.o: motor.c
	gcc -c motor.c
compass.o: compass.c
	gcc -c compass.c
mitibiki.o: mitibiki.c
	gcc -c mitibiki.c
