#include <stdio.h>
#include <wiringPi.h>
#include "gut.h"

static const int FIRST_GUT_PIN = 10;
static const int SECOND_GUT_PIN = 9;
static const int CURRENT_ON_TIME = 3000; //ms・3秒間
static const int CURRENT_OFF_TIME = 2000; //ms・2秒間

int cut_initialize()
{
	if(wiringPiSetupGpio() == -1)
	{
		printf("error wiringPi setup\n");
		return -1;
	}
	else
	{
		printf("wiringPisetup success\n");
	}
	pinMode(FIRST_GUT_PIN, OUTPUT);
	pinMode(SECOND_GUT_PIN, OUTPUT);
	return 0;
}

//NOTE HIGHとLOWは1と0に元から割り当てられてる
int cutInterupt(void)
{
	digitalWrite(FIRST_GUT_PIN,LOW);
	digitalWrite(SECOND_GUT_PIN,LOW);
	return 0;
}

static int cut(int gut_pin)
{
	int i;
	for(i=0; i<2; i++)
	{
		//念のため二回流す
		printf("count:%d\n", i+1);
		digitalWrite(gut_pin,HIGH);
		delay(CURRENT_ON_TIME);
		digitalWrite(gut_pin,LOW);
		delay(CURRENT_OFF_TIME);
	}
	return 0;
}

int cut_all()
{
	printf("first cut start\n");
	cut(FIRST_GUT_PIN);
	printf("first cut end\n");
	printf("second cut start\n");
	cut(SECOND_GUT_PIN);
	printf("second cut end\n");
	return 0;
}
