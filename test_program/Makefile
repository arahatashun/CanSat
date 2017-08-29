LIBS =-lwiringPi

motor:
	gcc -o motor.out motor_test.c ../motor.c ../mitibiki.c ../compass.c $(LIBS) -lm

gut:
	gcc -o gut.out gut_test.c ../gut.c $(LIBS)

lux:
	gcc -o lux.out luxsensor_test.c ../luxsensor.c $(LIBS)

accl:
	gcc -o accl.out accl_test.c ../acclgyro.c $(LIBS) -lm

gyro:
	gcc -o gyro.out gyro_test.c ../acclgyro.c $(LIBS) -lm

xbee:xbee_test.c
	gcc -o xbee.out xbee_test.c ../xbee_at.c $(LIBS) -lgps -lm

gps:gps_test.c
	gcc -o gps.out gps_test.c $(LIBS) -lgps -lm

bme280:bme280.o ../bme280.c
	gcc -o bme280.out bme280_test.c bme280.o $(LIBS) -lm

sealevel:bme280.o
	gcc -o sealevel.out sealevel.c bme280.o $(LIBS) -lm

escape:
	gcc -o escape.out escape.c ../motor.c ../compass.c ../mitibiki.c $(LIBS) -lm

gyromotor:
	gcc -o  gyromotor.out gyromotor.c ../motor.c  ../acclgyro.c ../pid.c $(LIBS) -lm

compass:motor.o
	gcc -o compass.out compass_test.c ../compass.c motor.o $(LIBS) -lm

bme280.o:../bme280.c
	gcc -c -Wall ../bme280.c $(LIBS) -lm

motor.o:
	gcc -c ../motor.c $(LIBS)

buffer:
	gcc -o buffer.out gps_havebuffer.c $(LIBS) -lgps -lm

nobuffer:
	gcc -o nobuffer.out gps_nobuffer.c $(LIBS) -lgps -lm
