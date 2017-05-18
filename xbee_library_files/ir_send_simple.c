/*********************************************************************
本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

                               Copyright (c) 2009 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/

// 各モード(AEHA/NEC/SIRC)毎に関数を用意している(mode設定の遅延を低減するために)
// 方式が固定の場合は不要な関数を削除する

#ifndef IR_SEND_S_H
	#define IR_SEND_S_H
	#include "ir_send_simple.h"
#endif

/* AEHA方式の赤外ＬＥＤ点滅用 */
void ir_flash(int times){
	int j,k;
	
	for(j=0 ; j < times * FLASH_AEHA_TIMES ; j++){
		IR_OUT = IR_OUT_ON;
		for(k=0 ; k < FLASH_AEHA_WAIT_H ; k++);
		IR_OUT = IR_OUT_OFF;
		for(k=0 ; k < FLASH_AEHA_WAIT_L ; k++);
	}
}
void ir_wait(int times){
	int j,k;
	
	for(j=0 ; j < times * FLASH_AEHA_TIMES ; j++){
		IR_OUT = IR_OUT_OFF;
		for(k=0 ; k < FLASH_AEHA_WAIT_H ; k++);
		IR_OUT = IR_OUT_OFF;
		for(k=0 ; k < FLASH_AEHA_WAIT_L ; k++);
	}
}

/* 信号送出（モード別に点滅関数を呼び出す） */
void ir_send_simple(byte *data, int data_len, enum Ir_Mode mode){
	int i,bit;
	byte out;

	if( mode == AEHA ){
		ir_flash(8);						// send 'SYNC'
		ir_wait(4);							// send 'SYNC'
		for(i=0;i<IR_DATA_SIZE;i++){
			out = data[i];
			for(bit=0;bit<8;bit++){
				ir_flash(1);
				if( ((out)&(1)) == 0){
					ir_wait((int)1);
				}else{
					ir_wait((int)3);
				}
				data_len--;
				if( data_len == 0){
					i = IR_DATA_SIZE -1;
					bit=7;
				}
				out >>= 1;
			}
		}
		ir_flash(1);						// send 'stop'
	}
}
