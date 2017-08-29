#include <stdio.h>
#include "../bme280.h"

int main()
{
	bme280_initialize();
	float altitude;
	printf("高度を入力してください\n");
	scanf("%f",&altitude);
	float sealevel = getSealevelPressure(altitude);
	printf("SEALEVEL PRESSURE  is %f",sealevel);
	return 0;
}
