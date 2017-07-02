#include <stdio.h>
#include <wirigPi.h>
#include "motor.h"
#include "gut.h"


int main(void)
{
	cutInterupt(void);
	motor_stop();
	delay(10);
	return 0;
}
