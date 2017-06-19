objs0 = parachute.o camera.o mitibiki.o

parachute.out: $(objs0)
	g++ -o parachute parachute.cpp $(objs0) `pkg-config --cflags opencv` `pkg-config --libs opencv`
parachute.o: parachute.cpp
	g++ -o parachute parachute.cpp
camera.o: camera.cpp
	g++ -o camera camera.cpp
