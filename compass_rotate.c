#include <stdio.h>
#include <signal.h>
#include <wiringPi.h>
#include "motor.h"
#include "compass.h"

//シグナルハンドラ
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
	while(1)
	{
		motor_escape(90);
	}
}
