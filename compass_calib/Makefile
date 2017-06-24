LIBS =-lwiringPi

scatter:
	gcc -o scatter.out compass_scatter.c ../compass.c $(LIBS)

test:
	gcc -o test.out calib_test.c ../compass.c $(LIBS)
