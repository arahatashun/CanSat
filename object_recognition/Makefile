objs0 = goal.o camera.o

goal.out: $(objs0)
	g++ -o goal.out $(objs0) `pkg-config --cflags opencv` `pkg-config --libs opencv`
goal.o: goal.cpp
	g++ -c goal.cpp
camera.o: camera.cpp
	g++ -c camera.cpp
