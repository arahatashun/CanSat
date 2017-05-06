# Makefile

LIBS =-lwiringPi -lm
objs = test_run.c motor.o compass.o

test: $(objs)
	gcc -Wall -O2 -o test $(objs) $(LIBS)
test_run.o: test_run.c
	gcc -c test_run.c
motor_run.o: motor.c
	gcc -c motor.c
compass.o: compass.c
	gcc -c compass.c
