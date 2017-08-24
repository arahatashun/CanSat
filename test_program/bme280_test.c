#include <stdio.h>
#include "../bme280.h"

int main()
{
	bme280_initialize();
	while (1)
	{
		readAltitude();
	}
	return 0;
}
