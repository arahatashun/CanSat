#include "compass.h"
#include "acclgyro.h"
#include <math.h>
#include <stdio.h>
#include <wiringPi.h>

static const double PI = 3.14159265359;

int main()
{
	double phi_rad = 0;
	double psi_rad = 0;
	double theta_degree = 0;
	Acclgyro acclgyro_data;
	Cmps compass_data;
	acclgyro_initializer();
	compass_initializer();
	while(1)
	{
		accl_and_rotation_read(&acclgyro_data);
		compass_read(&compass_data);
		phi_rad = cal_roll(acclgyro_data.acclY_scaled, acclgyro_data.acclZ_scaled);
		psi_rad = cal_pitch(acclgyro_data.acclX_scaled, acclgyro_data.acclY_scaled,
		                    acclgyro_data.acclZ_scaled, phi_rad);
		printf("phi_degree = %f\n", phi_rad*180.0/PI);
		printf("psi_degree = %f\n", psi_rad*180.0/PI);
		double theta_degree1 = cal_deg_acclcompass(compass_data.compassx_value,compass_data.compassy_value,
		                                           compass_data.compassz_value,sin(phi_rad),
		                                           sin(psi_rad),cos(phi_rad),cos(psi_rad));
		double theta_degree2 = cal_theta(theta_degree1);//値域が0~360になるように計算
		theta_degree = cal_deviated_angle(theta_degree2);
		printf("theta_degree = %f\n", theta_degree);
		delay(1000);
	}
}
