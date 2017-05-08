// acclgyro.h

#ifndef ACCLGYRO_H
#define ACCLGYRO_H

typedef struct acclgyro{
	double acclX_scaled, acclY_scaled, acclZ_scaled;	//the values of accleration 
	double gyroX_scaled, gyroY_scaled, gyroZ_scaled;    //the values of gyroscope
	double x_rotation,y_rotation;    //XY-axis rotation
	} Acclgyro;
	
int read_word_2c(int addr);    
double dist(double a,double b);
double get_y_rotation(double x,double y,double z);
double get_x_rotation(double x,double y,double z);
void accl_and_rotation_read(Acclgyro *acg);    //acgは構造体オブジェクトをさすポインタ
void gyro_read(Acclgyro *acg);

void init_acclgyro();   //initialization
void set_acclgyro(Acclgyro *acg);    //integrate accl_read,gyro_read,rotation_read
void print_acclgyro(Acclgyro *acg);    //print acclgyro parameter
int z_posture(Acclgyro *acg);

#endif



	
