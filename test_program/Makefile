LIBS =-lwiringPi

motor:
	gcc -o motor.out motor_test.c ../motor.c $(LIBS)

gut:
	gcc -o gut.out gut_test.c ../gut.c $(LIBS)

lux:
	gcc -o lux.out luxsensor_test.c ../luxsensor.c $(LIBS)

gyro:
	gcc -o gyro.out gyro_test.c ../acclgyro.c $(LIBS) -lm

xbee:
	gcc -o xbee.out xbee_test.c ../xbee_at.c $(LIBS) -lgps -lm

gps:
	gcc -o gps.out gps_test.c $(LIBS) -lgps -lm

escape:
	gcc -o escape.out escape.c ../motor.c $(LIBS)

gyromotor:
	gcc -o  gyromotor.out gyromotor.c ../motor.c  ../acclgyro.c ../pid.c $(LIBS) -lm

compass:
	gcc -o compass.out compass_test.c ../compass.c $(LIBS)
