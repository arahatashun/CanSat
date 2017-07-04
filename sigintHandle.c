#include <stdio.h>
#include <wiringPi.h>
#include "motor.h"
#include "gut.h"


int main(void)
{
	pwm_initialize();
	cut_initialize();
	printf("Executing Sigint Handle\n");
	cutInterupt();
	motor_stop();
	delay(1000);
	printf("Sigint Handle FINISHED\n");
	return 0;
}
