#include <stdio.h>
#include <wiringPi.h>
#include "compass.h"
#include "motor.h"
#include "pid.h"
#include <signal.h>
#include <stdlib.h>

static const double COMPASS_X_OFFSET = -92.0; //ここに手動でキャリブレーションしたoffset値を代入
static const double COMPASS_Y_OFFSET = -253.5;
static const int SETPOINT = 0.0;//delta_angleの目標値
static const double KP_VALUE= 0.5;
static const double KI_VALUE = 0.0005;
static const double KD_VALUE = 0;

void handler(int signum)
{
	motor_stop();
	delay(100);
	exit(1);
}

double cal_compass_theta()
{
	Cmps compass_data;
	compass_value_initialize(&compass_data);
	compass_read(&compass_data);
	double compass_x = 0;
	double compass_y = 0;
	//NOTE ここは地磁気が抜けていると無限ループに入りかねないのでそのうちGPS制御に移りたい
	while(compass_data.x_value == -1.0 && compass_data.y_value == -1.0)//地磁気resister　error
	{
		handle_compass_error(&compass_data);
		printf("\n");
	}
	compass_x = compass_data.x_value - COMPASS_X_OFFSET;
	compass_y = compass_data.y_value - COMPASS_Y_OFFSET;
	printf("compass_degree = %f\n",calc_compass_angle(compass_x, compass_y));
	double delta_angle = 0;
	if(calc_compass_angle(compass_x, compass_y) > 180)
	{
		delta_angle = calc_compass_angle(compass_x, compass_y) - 360;
	}
	else
	{
		delta_angle = calc_compass_angle(compass_x, compass_y);
	}
	return delta_angle;
}

int main()
{
	signal(SIGINT, handler);
	pwm_initialize();
	compass_initialize();
	Pid pid_data;
	while(1)
	{
		int i = 0;
		double compass_angle = 0;
		pid_initialize(&pid_data);
		pid_const_initialize(&pid_data,SETPOINT,KP_VALUE,KI_VALUE,KD_VALUE);
		pid_data.setpoint = 0.0;
		for(i=0; i<20; i++)
		{
			compass_angle = cal_compass_theta();
			pid_data.input = (int)(compass_angle);
			compute_output(&pid_data);
			printf("pid_output = %d\n",pid_data.output);
			motor_slalom(pid_data.output);
			delay(50);
			if(i==19)
			{
				printf("integral finish\n");
			}
		}
	}
}
