#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <wiringPi.h>
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
		isReverse();
		delay(100);
	}
}
