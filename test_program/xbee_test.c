#include <stdio.h>
#include <wiringPi.h>
#include <gps.h>
#include "../xbee_at.h"

int main()
{
  xbee_init();
  gps_init();
  while (1)
  {
    loc_t data;
    gps_location(&data);
		xbeePrintf("latitude:%f\nlongitude:%f\n", data.latitude, data.longitude);
    delay(1000);
  }
}
