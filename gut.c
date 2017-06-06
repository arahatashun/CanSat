//gut.c

#include<stdio.h>
#include<wiringPi.h>
#include"gut.h"

static const int gut_pin = 0;
static const int electric_on_time = 4000; //ms,4秒間

int cut_initializer(){
    if(wiringPiSetup() == -1){
         printf("error wiringPi setup\n");
         return -1;
        }
    pinMode(gut_pin, OUTPUT);
    return 0;
}

int cut(){
    int i=0;
    for(i=0;i<2;i++){
      //念のため二回流す
      digitalWrite(gut_pin,1);
      delay(electric_on_time);
      digitalWrite(gut_pin,0);
      delay(10);
    }
    return 0;
}
