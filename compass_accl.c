#include "compass.h"
#include "acclgyro.h"
#include <math.h>
#include <stdio.h>

static const double PI = 3.14159265359;
static const double convert_to_G = 16384.0;

double cal_theta(double theta_atan2)
{
    double theta;
    theta = theta_atan2;
    if(theta < 0)
    {
        theta = 360 - ((-1.0)*theta);
    }
    else
    {
        theta = 360 -((-1.0)*theta + 360);
    }
    return theta;
}

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
    double y1 = 0;
    double y2 = 0;
    double x1 = 0;
    double x2 = 0;
    double x3 = 0;
    double theta_degree = 0;
    while(1)
    {
      acclx_knd = (double)get_acclx()/convert_to_G*0.1 + acclx_knd*0.9;
      accly_knd = (double)get_accly()/convert_to_G*0.1 + accly_knd*0.9;
      acclz_knd = (double)get_acclz()/convert_to_G*0.1 + acclz_knd*0.9;
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
      printf("phi_degree = %f\n", phi_degree);
      printf("psi_degree = %f\n", psi_degree);
      y1 = zcompass_knd*sin(phi_radian);
      y2 = ycompass_knd*cos(phi_radian);
      x1 = xcompass_knd*cos(psi_radian);
      x2 = ycompass_knd*sin(psi_radian)*sin(phi_radian);
      x3 = zcompass_knd*sin(psi_radian)*cos(phi_radian);
      theta_degree = atan2(y1 - y2,x1 + x2 + x3)*180.0/PI;
      theta_degree = cal_theta(theta_degree);
      printf("theta_degree = %f\n", theta_degree);
      delay(1000);
    }
}
