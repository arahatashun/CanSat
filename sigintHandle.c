#include <stdio.h>
#include <wiringPi.h>
#include "motor.h"
#include "gut.h"


int main(void)
{
	cutInterupt();
	motor_stop();
	delay(10);
	return 0;
}
