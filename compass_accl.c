#include "compass.h"
#include "acclgyro.h"
#include <math.h>
#include <stdio.h>

static const double PI = 3.14159265359;
static const double convert_to_G = 16384.0;
static const int angle_of_deviation = -7;
/*
   ロール角を計算
 */
double cal_roll(double y,double z)
{
	return atan2(y,z);
}
/*
   ピッチ角を計算
 */
double cal_pitch(double x,double y,double z,double phi)
{
	return atan2(-x, y*sin(phi) + z*cos(phi));
}
/*
   下のmain関数で得られたtheta_degreeを実際の角度(0~360)に変換
 */
double cal_theta(double theta_atan2)
{
	double theta;
	theta = theta_atan2;
	if(theta < 0)
	{
		theta = 360 + theta;
	}
	else
	{
		theta = theta;
	}
	return theta;
}

/*
   偏角-7を考慮
 */
double cal_deviated_angle(double theta_degree)
{
	double true_theta = 0;
	true_theta = theta_degree + angle_of_deviation;
	if (true_theta > 360)
	{
		true_theta = true_theta - 360;
	}
	else if(true_theta<0)
	{
		true_theta = true_theta+ 360;
	}
	else
	{
		true_theta = true_theta;
	}
	return true_theta;
}
int main()
{
	double acclx = 0;
	double accly = 0;
	double acclz = 0;
	double xcompass = 0;
	double ycompass = 0;
	double zcompass = 0;
	double phi_radian = 0;
	double psi_radian = 0;
	double phi_degree = 0;
	double psi_degree = 0;
	double y1 = 0;
	double y2 = 0;
	double x1 = 0;
	double x2 = 0;
	double x3 = 0;
	double theta_degree = 0;
	Acclgyro acclgyro_data;
	Cmps compass_data;
	acclgyro_initializer();
	compass_initializer();

	while(1)
	{
		accl_and_rotation_read(&acclgyro_data);
		compass_read(&compass_data);
		acclx = (double) acclgyro_data.acclX_scaled/convert_to_G*0.1 + acclx*0.9;
		accly = (double) acclgyro_data.acclY_scaled/convert_to_G*0.1 + accly*0.9;
		acclz = (double) acclgyro_data.acclZ_scaled/convert_to_G*0.1 + acclz*0.9;
		xcompass = (double)compass_data.compassx_value;
		ycompass = (double)compass_data.compassy_value;
		zcompass = (double)compass_data.compassz_value;
		printf("acclx = %f\n", acclx);
		printf("accly = %f\n", accly);
		printf("acclz = %f\n", acclz);
		printf("compassx = %f\n", xcompass);
		printf("compassy = %f\n", ycompass);
		printf("compassz = %f\n", zcompass);
		phi_radian = cal_roll(accly, acclz);
		psi_radian = cal_pitch(acclx, accly, acclz, phi_radian);
		phi_degree = phi_radian*180.0/PI;
		psi_degree = psi_radian*180.0/PI;
		printf("phi_degree = %f\n", phi_degree);
		printf("psi_degree = %f\n", psi_degree);
		y1 = zcompass*sin(phi_radian);
		y2 = ycompass*cos(phi_radian);
		x1 = xcompass*cos(psi_radian);
		x2 = ycompass*sin(psi_radian)*sin(phi_radian);
		x3 = zcompass*sin(psi_radian)*cos(phi_radian);
		theta_degree = atan2(y1 - y2,x1 + x2 + x3)*180.0/PI;
		theta_degree = cal_theta(theta_degree);
		theta_degree = cal_deviated_angle(theta_degree);
		printf("theta_degree = %f\n", theta_degree);
		delay(1000);
	}
}
