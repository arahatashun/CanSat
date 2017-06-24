#include <stdio.h>
#include <wiringPi.h>
#include "compass.h"
#include "motor.h"
#include "pid.h"
#include <signal.h>
#include <stdlib.h>

static const double COMPASS_X_OFFSET = 0.0; //ここに手動でキャリブレーションしたoffset値を代入
static const double COMPASS_Y_OFFSET = 0.0;

void handler(int signum)
{
	motor_stop();
	delay(100);
	exit(1);
}

int cal_compass_theta()
{
	Cmps compass_data;
	compass_value_initialize(&compass_data);
	compass_mean(&compass_data);
	double compass_x = 0;
	double compass_y = 0;
	//NOTE ここは地磁気が抜けていると無限ループに入りかねないのでそのうちGPS制御に移りたい
	while(compass_data.x_value == -1.0 && compass_data.y_value == -1.0) //地磁気resister　error
	{
		handle_compass_error();
		delay(1000);
		compass_mean(&compass_data);
		printf("\n");
	}
	print_compass(&compass_data);
	compass_x = compass_data.x_value - COMPASS_X_OFFSET;
	compass_y = compass_data.y_value - COMPASS_Y_OFFSET;
	printf("compass_degree = %f\n",calc_compass_angle(compass_x, compass_y));
	return 0;
}

int main()
{
	signal(SIGINT, handler);
	pwm_initialize();
	compass_initialize();
	while(1)
	{
		motor_forward(100);
		cal_compass_theta();
		delay(1000);
		motor_stop();
		delay(1000);
	}
}
