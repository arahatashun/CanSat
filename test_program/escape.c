#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <math.h>
#include "../motor.h"
#include "../compass.h"
#include "../mitibiki.h"

static const int STACK_COUNTER = 20;

void handler(int signum)
{
	motor_stop();
	delay(100);
	exit(1);
}

int motor_rotate_compass(double angle_to_rotate)
{
	int c = 0;  //行きたい方角に回転できなくても無限ループにならないようにカウンター用意
	double delta_angle = 180;
	double compass_angle_fixed =readCompassAngle();
	double target_angle = cal_deviated_angle(0, compass_angle_fixed + angle_to_rotate);
	while(fabs(delta_angle) > 30 && c <STACK_COUNTER)
	{
		double compass_angle =readCompassAngle();
		delta_angle= cal_delta_angle(compass_angle,target_angle);
		printf("delta_angle: %f\n", delta_angle);
		if(delta_angle>0)
		{
			motor_right(100);
		}
		else
		{
			motor_left(100);
		}
		delay(50);
		c++;
	}
	if(c >= STACK_COUNTER)
	{
		printf("could not escape\n");
	}

	motor_stop();
	delay(2000);
	return 0;
}

int motor_escape()
{
	int i;
	for(i=1; i<6; i++)
	{
		motor_back(i*20);
		delay(200);
	}
	printf("get stacked\n");
	motor_rotate_compass(90);
	motor_rotate_compass(-45);
	return 0;
}


int main()
{
	signal(SIGINT, handler);
	pwm_initialize();
	while(1)
	{
		motor_escape();
	}
}
