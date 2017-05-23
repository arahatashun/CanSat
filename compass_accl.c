#include "compass.h"
#include "acclgyro.h"
#include <math.h>
#include <stdio.h>
#include <wiringPiI2C.h>

static const double PI = 3.14159265359;
static const double convert_to_G = 16384.0;

int main()
{
  	acclgyro_initializer();
    compass_initializer_knd();
    double acclx_knd = 0;
    double accly_knd = 0;
    double acclz_knd = 0;
    double xcompass_knd = 0;
    double ycompass_knd = 0;
    double zcompass_knd = 0;
    double phi_radian = 0;
    double psi_radian = 0;
    double phi_degree = 0;
    double psi_degree = 0;
    double atan_y = 0;
    double atan_x = 0;
    double theta_degree = 0;
    while(1)
    {
      acclx_knd = (double)get_acclx()/convert_to_G;
      accly_knd = (double)get_accly()/convert_to_G;
      acclz_knd = (double)get_acclz()/convert_to_G;
      xcompass_knd = (double)get_xcompass();
      ycompass_knd = (double)get_ycompass();
      zcompass_knd = (double)get_zcompass();
      printf("acclx = %lf\n", acclx_knd);
      printf("accly = %lf\n", accly_knd);
      printf("acclz = %lf\n", acclz_knd);
      printf("compassx = %lf\n", xcompass_knd);
      printf("compassy = %lf\n", ycompass_knd);
      printf("compassz = %lf\n", zcompass_knd);
      phi_radian = atan2(accly_knd, acclz_knd);
      psi_radian = atan2(-acclx_knd, accly_knd*sin(phi_radian) + acclz_knd*cos(phi_radian));
      phi_degree = phi_radian*180.0/PI;
      psi_degree = psi_radian*180.0/PI;
      printf("phi = %lf\n", phi_degree);
      printf("psi = %lf\n", psi_degree);
      printf("sin(phi) = %lf\n", sin(phi_radian));
      printf("cos(phi) = %lf\n", cos(phi_radian));
      atan_y = 10000*acclz_knd*sin(phi_radian);/*- 10000*accly_knd*cos(phi_radian)
      atan_x = acclx_knd*cos(psi_radian) + accly_knd*sin(psi_radian)*sin(phi_radian) + acclz_knd*sin(psi_radian)*cos(phi_radian);
      printf("atan_y = %lf", atan_y);
      printf("atan_x = %lf", atan_x);*/
      printf("atan_y = %lf", atan_y);
      atan_y = 2.0;
      printf("atan_y = %lf", atan_y);
      atan_y = accly_knd*cos(phi_radian)*10000;
      printf("atan_y = %lf", atan_y);
        
      theta_degree = atan2(atan_y, atan_x)*(180.0/PI)+ 180.0;
      printf("theta = %lf\n", theta_degree);
        delay(1000);
    }
    return 0;
}
