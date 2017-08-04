#include <stdio.h>
#include <math.h>
#include <wiringPiI2C.h>
#include "../acclgyro.h"

int main()
{
	Accl accl_data;
	acclGyro_initialize();
	while(1)
	{
		Accl_read(&accl_data);
		printf("x_accl is %f\n",accl_data.acclX_scaled);
		printf("y_accl is %f\n",accl_data.acclY_scaled);
		printf("z_accl is %f\n",accl_data.acclZ_scaled);
		isReverse();
		delay(1000);
	}

	return 0;
}
