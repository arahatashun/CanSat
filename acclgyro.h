// acclgyro.h

#ifndef ACCLGYRO_H
#define ACCLGYRO_H

//構造体宣言(typedef)
typedef struct acclgyro {
  double acclX_scaled;//the values of accleration
  double acclY_scaled;
  double acclZ_scaled;
  double gyroX_scaled;
  double gyroY_scaled;
  double gyroZ_scaled;//the values of gyroscope
  double x_rotation;
  double y_rotation;//XY-axis rotation
} Acclgyro;

int acclgyro_initializer();   //initialization
int print_acclgyro(Acclgyro *acclgyro_data);    //六軸センサーの値を返す
int is_reverse(Acclgyro *acclgyro_data);  //反転していたら1を返す

#endif
