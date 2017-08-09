#include <stdio.h>
#include <signal.h>
#include <wiringPi.h>
#include <math.h>
#include <stdlib.h>
#include "motor.h"
#include "acclgyro.h"

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
	acclGyro_initialize();
	while(1)
	{
		motor_right(100);
		isReverse();
		delay(100);
	}
}