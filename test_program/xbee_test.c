#include <stdio.h>
#include <wiringPi.h>
#include <gps.h>
#include "../xbee_at.h"

int main()
{
  xbee_init();
  gps_init();
  int i=0;
  while (1)
  {
    loc_t data;
    gps_location(&data);
    xbeePrintf("COUNTER : %d",++i);
		xbeePrintf("latitude:%f\nlongitude:%f\n", data.latitude, data.longitude);
    delay(1000);
  }
}
