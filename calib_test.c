/*#include <wiringPi.h>
 #include "compass.h"
 #include <stdio.h>

   int main()
   {
        pwm_initializer();
        compass_initializer();
        Cmps compass_data;
        Cmps_offset compass_offset;
        cal_maxmin_compass(&compass_offset,&compass_data);
        delay(2000);
        double compass_x = 0;
        double compass_y = 0;
        double compass_angle = 0;
        while(1)
        {
                compass_read(&compass_data);
                compass_x = compass_data.compassx_value - compass_offset.compassx_offset;
                compass_y = compass_data.compassy_value - compass_offset.compassy_offset;
                compass_angle = calc_compass_angle(compass_x, compass_y);
                printf("compass_angle=%f\n",compass_angle);
                delay(1000);
        }
        return 0;
   }
 */
