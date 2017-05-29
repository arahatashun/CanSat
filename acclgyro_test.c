#include <stdio.h>
#include <math.h>
#include <wiringPiI2C.h>
#include "acclgyro.h"

int main(){
	Acclgyro acclgyro_data; //構造体オブジェクト生成
	acclgyro_initializer();
	while(1)
	{
		print_acclgyro(&acclgyro_data);
		is_reverse(&acclgyro_data);
		delay(1000);
	}
	return 0;
}
