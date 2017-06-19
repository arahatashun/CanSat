objs0 = goal.o camera.o motor.c
LIBS =-lwiringPi

goal.out: $(objs0)
	g++ -o goal.out $(objs0) `pkg-config --cflags opencv` `pkg-config --libs opencv` $(LIBS)
goal.o: goal.cpp
	g++ -c goal.cpp
camera.o: camera.cpp
	g++ -c camera.cpp
motor.o: ../motor.c
	gcc -c motor.c
