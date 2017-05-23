#include "compass.h"
#include "acclgyro.h"
#include <math.h>
#include <stdio.h>
#include <wiringPiI2C.h>

static const double PI = 3.14159265359;


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
    double theta_degree = 0;
    while(1)
    {
      acclx_knd = acclx_knd*0.9 + ((double)get_acclx())*0.1;
      accly_knd = accly_knd*0.9 + ((double)get_accly())*0.1;
      acclz_knd = acclz_knd*0.9 + ((double)get_acclz())*0.1;
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
      printf("theta = %lf\n", phi_degree);
      printf("phi = %lf\n", psi_degree);
      theta_degree = atan2(acclz_knd*sin(phi_radian) - accly_knd*cos(phi_radian), acclx_knd*cos(psi_radian) + accly_knd*sin(psi_radian)*sin(phi_radian) + acclz_knd*sin(psi_radian)*cos(phi_radian)) * 180.0/PI + 180.0;
      printf("theta = %lf\n", theta_degree);
        delay(1000);
    }
    return 0;
}
