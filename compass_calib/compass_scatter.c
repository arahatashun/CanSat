#include <wiringPi.h>
#include "../compass.h"
#include <stdio.h>

int main()
{
	while(1)
	{
		read_for_calib();
		delay(200);
	}
	return 0;
}
