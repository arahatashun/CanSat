#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "motor.h"
#include "acclgyro.h"



int main()
{
	signal(SIGINT, handler);
	pwm_initialize();
	acclGyro_initialize(void);
	while(1)
	{
		is_Reverse();
		delay(100);
	}
}
