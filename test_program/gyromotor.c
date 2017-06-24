#include <signal.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <stdio.h>
#include "../motor.h"
#include "../acclgyro.h"


void handler(int signum)
{
	motor_stop();
	delay(100);
	exit(1);
}



int main()
{
	signal(SIGINT, handler);
	Accl accl_data;
	Gyro gyro_data;
	acclGyro_initialize();
	pwm_initialize();
	int now = 0;
	int lastTime = millis();
	double delta_theta = 0;
	while(1)
	{
		readGyro(&gyro_data);
		now = millis();
		int delta_time = now-lastTime;
		delta_theta += gyro_data.gyroZ_scaled*delta_time/1000;//convert2seconds
		printf("%f\n",delta_theta);
		lastTime = now;
		double rotate_power = - delta_theta + 90;
		printf("%f\n",rotate_power);
		motor_rotate(-rotate_power);
	}
	return 0;
}
