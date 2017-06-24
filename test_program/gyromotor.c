#include <signal.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "../motor.h"
#include "../acclgyro.h"

int main()
{
	Accl accl_data;
	Gyro gyro_data;
	acclGyro_initialize();
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
		motor_rotate(delat_theta-90);
	}
}
