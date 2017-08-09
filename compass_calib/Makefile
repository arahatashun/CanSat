LIBS =-lwiringPi -lm

scatter:
	gcc -o scatter.out compass_scatter.c ../compass.c ../motor.c ../mitibiki.c $(LIBS)

test:
	gcc -o test.out calib_test.c ../compass.c ../motor.c $(LIBS)
