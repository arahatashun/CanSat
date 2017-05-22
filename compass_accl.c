#include "compass.h"
#include "acclgyro.h"
#include <math.h>
#include <stdio.h>

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
    double theta = 0;
    double phi = 0;

    while(1)
    {
      acclx_knd = (double)get_acclx();
      accly_knd = (double)get_accly();
      acclz_knd = (double)get_acclz();
      xcompass_knd = (double)get_xcompass();
      ycompass_knd = (double)get_ycompass();
      zcompass_knd = (double)get_zcompass();
      printf("acclx = %lf", acclx_knd);
      printf("accly = %lf", accly_knd);
      printf("acclz = %lf", acclz_knd);
      printf("compassx = %lf", xcompass_knd);
      printf("compassy = %lf", ycompass_knd);
      printf("compassz = %lf", zcompass_knd);
      theta = atan2(accly_knd, acclz_knd);
      phi = atan2(-acclx_knd, accly_knd*sin(theta) + acclz_knd*cos(theta));
      printf("theta = %lf", theta);
      printf("phi = %lf", phi);
    }
}
