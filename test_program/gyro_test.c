#include <stdio.h>
#include <math.h>
#include <wiringPiI2C.h>
#include "../acclgyro.h"

int main(){
	Accl accl_data;
	Gyro gyro_data;
	acclgyro_initialize();
	int now = 0;
	int lastTime = millis();
	double delat_theta = 0;
	while(1)
	{
		readGyro(&gyro_data);
		now = millis();
		int delta_time = now-lastTime;
		delat_theta += gyro_data.gyroZ_scaled*delta_time/1000;//convert2seconds
		printf("%f\n",delat_theta);
		lastTime = now;
	}

	return 0;
}
