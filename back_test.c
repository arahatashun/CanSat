#include <signal.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "motor.h"

void handler(int signum)
{
	motor_stop();
	delay(100);
	exit(1);
}


int main()
{
	int i;
	signal(SIGINT, handler);
	pwm_initialize();
	motor_forward(40);
	delay(1000);
	motor_forward(200);
	motor_forward(100);
	delay(2000);
	/*
	   for(i=2; i<5; i++)
	   {
	        motor_back(100-20*i);
	        delay(100);
	   }*/
	motor_stop();
	delay(100);

}
