// acclgyro.h

#ifndef ACCLGYRO_H
#define ACCLGYRO_H

int acclgyro_initializer();   //initialization
int print_acclgyro(Acclgyro *acclgyro_data);    //六軸センサーの値を返す
int is_reverse(Acclgyro *acclgyro_data);  //反転していたら1を返す

#endif
