//xbee_luxsensor test program

#include"luxsensor.h"
#include"xbee_uart.h"
#include<stdio.h>

int main(){
    xbee_initializer();
    luxsensor_initializer();
    while(1){
    islight();
    }
    return 0;
   }
