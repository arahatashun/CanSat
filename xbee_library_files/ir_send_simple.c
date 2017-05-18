/*********************************************************************
�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

                               Copyright (c) 2009 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/

// �e���[�h(AEHA/NEC/SIRC)���Ɋ֐���p�ӂ��Ă���(mode�ݒ�̒x����ጸ���邽�߂�)
// �������Œ�̏ꍇ�͕s�v�Ȋ֐����폜����

#ifndef IR_SEND_S_H
	#define IR_SEND_S_H
	#include "ir_send_simple.h"
#endif

/* AEHA�����̐ԊO�k�d�c�_�ŗp */
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

/* �M�����o�i���[�h�ʂɓ_�Ŋ֐����Ăяo���j */
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
