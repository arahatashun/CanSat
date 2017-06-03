#include "xbeelib.h"
#include "xbeeuart.h"
#include<stdio.h>

static byte com=0xB0;                              // 拡張IOコネクタの場合は0xA0
static byte dev[]={0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF};

int xbee_initializer(){
  xbee_init( com );                           // XBee用COMポートの初期化
  printf("Waiting for XBee Commissoning\n");  // 待ち受け中の表示
  if(xbee_atnj(15) != 0){                     // デバイスの参加受け入れを開始
      printf("Found a Device\n");             // XBee子機デバイスの発見表示
      xbee_from( dev );                       // 見つけた子機のアドレスを変数devへ
      xbee_ratnj(dev,0);                      // 子機に対して孫機の受け入れ制限を設定
    }
  else{                                      // 子機が見つからなかった場合
      printf("no Devices\n");                 // 見つからなかったことを表示
    }
  return 0;
}

int xbee_print_char(char *input){
  xbee_uart_char(dev,input);
  return 0;
}

int xbee_print_int(int input){
  xbee_uart_int(dev,input);
  return 0;
}

int xbee_print_double(double input){
  xbee_uart_double(dev,input);
  return 0;
}

int xbee_print_double(double input){
  printf("%f\n",input);
  xbee_uart_double(dev,input);
  return 0;
}
