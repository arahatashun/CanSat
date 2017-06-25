# Makefile
LIBS = -lwiringPi -lm -lgps
objs0 = motor.o mitibiki.o pid.o ring_buffer.o

integration.out: $(objs0)
	gcc -g -Wall -O2 -o integration.out integration.o $(objs0) $(LIBS)

integration_knd3.out: $(objs0)
		gcc -g -Wall -O2 -o integration_knd3.out integration_knd3.o $(objs0) $(LIBS)

integration_knd3.o: integration_knd3.c
		gcc -c -Wall integration_knd3.c

integration.o: integration.c
	gcc -c integration.c

mitibiki.o: mitibiki.c
	gcc -c mitibiki.c

motor.o: motor.c
	gcc -c motor.c

pid.o: pid.c
	gcc -c pid.c

ring_buffer.o: ring_buffer.c
	gcc -c ring_buffer.c
