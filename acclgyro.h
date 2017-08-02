#ifndef ACCLGYRO_H
#define ACCLGYRO_H

#ifdef __cplusplus
extern "C" {
#endif

//三軸加速度(単位G)
typedef struct accl {
	double acclX_scaled;//the values of accleration
	double acclY_scaled;
	double acclZ_scaled;
}Accl;

//角速度 degrees/second
//NOTE z軸が上なので基本的にそれをみると良い
typedef struct gyro {
	double gyroX_scaled;//the values of accleration
	double gyroY_scaled;
	double gyroZ_scaled;
}Gyro;

int acclGyro_initialize(void);
int Accl_read(Accl* data);
int isReverse(void);
double cal_roll(Accl* data);
double cal_pitch(Accl* data);

#ifdef __cplusplus
}
#endif

#endif
