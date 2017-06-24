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
	while(1)
	{
		compass_initialize();
		readAccl(&acclgyro_data);
		compass_read(&compass_data);
		print_compass(&compass_data);
		phi_rad = cal_roll(acclgyro_data.acclY_scaled, acclgyro_data.acclZ_scaled);
		psi_rad = cal_pitch(acclgyro_data.acclX_scaled, acclgyro_data.acclY_scaled,
		                    acclgyro_data.acclZ_scaled, phi_rad);
		printf("phi_degree = %f\n", phi_rad*180.0/PI);
		printf("psi_degree = %f\n", psi_rad*180.0/PI);
		double theta_degree = cal_deg_acclcompass(compass_data.x_value,compass_data.y_value,
		                                          compass_data.z_value,sin(phi_rad),
		                                          sin(psi_rad),cos(phi_rad),cos(psi_rad));
		printf("theta_degree = %f\n", theta_degree);
		delay(1000);
	}
}
