#include <stdio.h>
#include <math.h>
#include <gps.h>
#include <wiringPi.h>

loc_t data;//gpsのデータを確認するものをグローバル変数宣言

int main()
{
	while(1)
	{
		gps_init();
		gps_location(&data);
		printf("latitude:%f\nlongitude:%f\naltitude:%f\n", data.latitude, data.longitude
		       ,data.altitude);
		delay(1000);
		gps_off();
	}
}
