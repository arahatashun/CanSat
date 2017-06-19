objs0 = goal.o camera.o

goal.out: $(objs0)
	g++ -o goal.out $(objs0) `pkg-config --cflags opencv` `pkg-config --libs opencv`
goal.o: goal.cpp
	g++ -o goal goal.cpp
camera.o: camera.cpp
	g++ -o camera camera.cpp
