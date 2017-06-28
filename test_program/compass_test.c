#include <stdio.h>
#include "../compass.h"

int main()
{
	int compass_initialize();
	while(1)
	{
		double angle =  readCompassAngle();
		printf("%f\n",angle);
	}
}
