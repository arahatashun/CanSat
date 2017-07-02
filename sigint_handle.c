#include <stdio.h>
#include <motor.h>
#include <gut.h>
#include <wirigPi.h>

int main(void)
{
	cutInterupt(void);
	motor_stop();
	delay(10);
	return 0;
}
