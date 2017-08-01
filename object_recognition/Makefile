objs0 = goal.o camera.o motor.o
LIBS =-lwiringPi

goal.out: $(objs0)
	g++ -o goal.out $(objs0) `pkg-config --cflags opencv` `pkg-config --libs opencv` $(LIBS)
test.out: $(objs0) test.cpp
	g++ -o test.out test.cpp $(objs0) pkg-config --cflags opencv` `pkg-config --libs opencv` $(LIBS)
goal.o: goal.cpp
	g++ -c goal.cpp
camera.o: camera.cpp
	g++ -c camera.cpp
motor.o: ../motor.c
	gcc -c ../motor.c
