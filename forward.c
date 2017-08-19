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
	signal(SIGINT, handler);
	pwm_initialize();
	motor_forward(100);
	delay(2000);
	motor_stop();
	delay(1000);
	return 0;
}
