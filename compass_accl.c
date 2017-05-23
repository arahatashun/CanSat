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
      printf("acclx = %f\n", acclx_knd);
      printf("accly = %f\n", accly_knd);
      printf("acclz = %f\n", acclz_knd);
      printf("compassx = %f\n", xcompass_knd);
      printf("compassy = %f\n", ycompass_knd);
      printf("compassz = %f\n", zcompass_knd);
      phi_radian = atan2(accly_knd, acclz_knd);
      psi_radian = atan2(-acclx_knd, accly_knd*sin(phi_radian) + acclz_knd*cos(phi_radian));
      phi_degree = phi_radian*180.0/PI;
      psi_degree = psi_radian*180.0/PI;
      printf("phi = %f\n", phi_degree);
      printf("psi = %f\n", psi_degree);
      printf("sin(phi) = %f\n", sin(phi_radian));
      printf("cos(phi) = %f\n", cos(phi_radian));
      atan_y =acclz_knd * sin(phi_radian);
      /*　-10000*accly_knd*cos(phi_radian)
      atan_x = acclx_knd*cos(psi_radian) + accly_knd*sin(psi_radian)*sin(phi_radian) + acclz_knd*sin(psi_radian)*cos(phi_radian);
      printf("atan_y = %f", atan_y);
      printf("atan_x = %f", atan_x);*/
      printf("atan_y = %f\n", atan_y);
      atan_y = accly_knd * cos(phi_radian);
      printf("atan_y = %f\n", atan_y);
      theta_degree = atan2(atan_y, atan_x)*(180.0/PI)+ 180.0;
      printf("theta = %f\n", theta_degree);
      delay(1000);
    }
    return 0;
}
