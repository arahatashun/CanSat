#include <stdio.h>
#include <wiringPi.h>
#include "gut.h"

static const int gut_pin = 10;
<<<<<<< HEAD
static const int electric_on_time = 3000; //ms・3秒間
=======
static const int electric_on_time = 3000; //ms・4秒間
>>>>>>> origin/compass4

int cut_initializer(){

	if(wiringPiSetupGpio() == -1)
	{
		printf("error wiringPi setup\n");
		return -1;
	}
	else
	{
		printf("wiringPisetup success\n");
	}
	pinMode(gut_pin, OUTPUT);
	return 0;
}

int cut(){
	int i=0;
	printf("cut now\n");
	for(i=0; i<2; i++) {
		//念のため二回流す
		printf("%d\n", i);
		digitalWrite(gut_pin,1);
		delay(electric_on_time);
		digitalWrite(gut_pin,0);
<<<<<<< HEAD
		delay(100);
=======
		delay(1000);
>>>>>>> origin/compass4
	}
	return 0;
}
