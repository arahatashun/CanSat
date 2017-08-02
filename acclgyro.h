#ifndef ACCLGYRO_H
#define ACCLGYRO_H

#ifdef __cplusplus
extern "C" {
#endif

int acclGyro_initialize(void);
int Gyro_read(Gyro *data);
int Accl_read(Accl*data);
int isReverse(void);
double cal_roll(Accl* data);
double cal_pitch(Accl* data);

#ifdef __cplusplus
}
#endif

#endif
