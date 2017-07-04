#include <stdio.h>
#include <wiringPi.h>
#include "motor.h"
#include "gut.h"


int main(void)
{
	printf("Executing Sigint Handle\n");
	cutInterupt();
	motor_stop();
	delay(100);
	printf("Sigint Handle FINISHED\n");
	return 0;
}
