//gut.c

#include<stdio.h>
#include<wiringPi.h>
#include"gut.h"

static const int gut_pin = 0;
static const int electric_on_time = 20000 //ms,20秒間

int cut(){
  if(wiringPiSetup() == -1){
        printf("error wiringPi setup\n");
        return 1;
    }
    pinMode(gut_pin, OUTPUT);
    for(i=0,i<3,i++){
      //念のため三回流す
      digitalWrite(gut_pin,1);
      delay(electric_on_time);
      digitalWrite(gut_pin,0);
      delay(10);
    }
}
