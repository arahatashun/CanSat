/*このコードは現在動きません*/

/*
 #include "compass.h"
 #include "acclgyro.h"
 #include <math.h>
 #include <stdio.h>
 #include <wiringPi.h>

   static const double PI = 3.14159265359;

   int main()
   {
        Accl accl_data;
        Cmps compass_data;
        acclGyro_initialize();
        compass_initialize();
        while(1)
        {
                readAccl(&accl_data);
                compass_read(&compass_data);
                double phi_rad = cal_roll(&accl_data);
                double psi_rad = cal_pitch(&accl_data);
                printf("phi_degree = %f\n", phi_rad*180.0/PI);
                printf("psi_degree = %f\n", psi_rad*180.0/PI);
                double theta_degree = cal_deg_acclcompass(compass_data.x_value,compass_data.y_value,
                                                          compass_data.z_value,sin(phi_rad),
                                                          sin(psi_rad),cos(phi_rad),cos(psi_rad));
                printf("theta_degree = %f\n", theta_degree);
                delay(1000);
        }
        return 0;
   }
 */
