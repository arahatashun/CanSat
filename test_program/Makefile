LIBS =-lwiringPi

motor:
	gcc -o motor.out motor_test.c ../motor.c $(LIBS)

gut:
	gcc -o gut.out gut_test.c ../gut.c $(LIBS)

lux:
	gcc -o lux.out luxsensor_test.c ../luxsensor.c $(LIBS)

gyro:
	gcc -o gyro.out gyro_test.c ../acclgyro.c $(LIBS)

xbee:
	gcc -o xbee.out xbee_test.c ../xbee_at.c  $(LIBS) -lgps

gps:
	gcc -o gps.out gps_test.c $(LIBS) -lgps
