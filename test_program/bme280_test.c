#include <stdio.h>
#include <time.h>
#include "../bme280.h"

int main()
{
	bme280_initialize();
	time_t startTime;
	time(&startTime);
	while (1)
	{
		time_t now;
		time(&now);
		printf("time = %d[s]\n", (int)(now-startTime));
		readAltitude();
	}
	return 0;
}
