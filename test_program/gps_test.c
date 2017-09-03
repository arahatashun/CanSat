#include <stdio.h>
#include <math.h>
#include <gps.h>
#include <time.h>
#include <wiringPi.h>

loc_t data;//gpsのデータを確認するものをグローバル変数宣言

int main()
{
	gps_init();
	time_t startTime;
	time(&startTime);
	while(1)
	{
		time_t now;
		time(&now);
		printf("time = %d[s]\n", (int)(now-startTime));
		gps_location(&data);
		printf("time:%f\nlatitude:%f\nlongitude:%f\naltitude:%f\n",
		       data.time,data.latitude, data.longitude,data.altitude);

	}
}
