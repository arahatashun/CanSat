//acclgyro_test.c

#include <stdio.h>
#include <math.h>
#include <wiringPiI2C.h>
#include "acclgyro.h"

int main()
{
	Acclgyro acg;    //オブジェクト生成
	init_acclgyro();
	while(1) {
		print_acclgyro(acg);
		delay(1000);
	}
	return 0;
}
