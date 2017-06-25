#include <signal.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <stdio.h>
#include "../motor.h"
#include "../acclgyro.h"
#include "../pid.h"


void handler(int signum)
{
	motor_stop();
	delay(100);
	exit(1);
}



int main()
{
	//Pid* pid = make_pid();
	Pid pid;
	pid_initialize(&pid);
	pid.Kp = 1;
	pid.Ki = 0.00007;
	pid.Kd = 0;
	pid.setpoint = 0;
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
		delta_theta -= gyro_data.gyroZ_scaled*delta_time/1000;//convert2seconds
		printf("delta theta %f\n",delta_theta);
		lastTime = now;
		pid.input = delta_theta;
		printf("pid input %f\n",pid.input);
		compute_output(&pid);
		printf("pid integral %f\n",pid.integral);
		printf("rotate power:%d\n",pid.output);
		motor_slalom(pid.output);
	}
	return 0;
}
