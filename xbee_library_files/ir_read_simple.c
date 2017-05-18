/*********************************************************************
�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

                               Copyright (c) 2009 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/

#ifndef IR_READ_S_H
	#define IR_READ_S_H
	#include "ir_read_simple.h"
#endif

/* �V���{���ǎ��*/
int ir_sens(int det){
	int count;
	unsigned char in,det_wait,det_count;
	
	if( det == IR_IN_OFF ){		/* AEHA, NEC */
		det_wait = IR_IN_ON;	// �����҂�
		det_count= IR_IN_OFF;	// �����J�E���g
	}else{						/* SIRC */
		det_wait = IR_IN_OFF;	// �_���҂�
		det_count= IR_IN_ON;	// �_���J�E���g
	}
	/* �҂� */
	in = IR_IN;
	count = IR_SYNC_WAIT;
	while( (count != 0) && (in == det_wait) ){
		in = IR_IN;
		count--;
	}
	/* �J�E���g */
	in = IR_IN;
	count = IR_SYNC_WAIT;
	while( (count != 0) && (in == det_count) ){
		in = IR_IN;
		count--;
	}
	if( count == 0 ){
		return( IR_SYNC_WAIT );
	}else{
		return( IR_SYNC_WAIT - count );
	}
}

/* �ԊO���M���ǂݎ��V���v�� */
int ir_read_simple(unsigned char *data, enum Ir_Mode mode){
	int i,bit;
	int data_len= IR_DATA_SIZE * 8;	/* Ir�f�[�^�̃r�b�g�� */
	int len, data_wait;
	int	len_on=0,len_off=0;			// �M����(���[�v�J�E���g)
	int symbol_len, noise;			// ����p�V���{����
	unsigned char det = IR_IN_OFF;	// ���莞�̓��͐M�����x��(SIRC�Ή�)
	unsigned char in;

	if( IR_IN == IR_IN_OFF) return(0);			/*�f�[�^�Ȃ�*/
	/* SYNC_ON���o */
	len_on = ir_sens(IR_IN_ON);
//	if( len_on == IR_SYNC_WAIT ) return(0);			/*�^�C���A�E�g*/
	if( len_on < 500 ) return(0);	
	/* SYNC_OFF���o */
	len_off = ir_sens(IR_IN_OFF);
//	if( len_off == IR_SYNC_WAIT) return(0);		/*�G���[*/
	if( len_on < 500 ) return(0);	

	/* ���[�h�ݒ�*/
	symbol_len = len_off/2;
	switch( mode ){
		case NA:
		case ERROR:
		case AEHA:	// H(8T) + L(4T)	2:1
			symbol_len = len_off/2;
			det=IR_IN_OFF;
			break;
		case NEC:	// H(16T) + L(8T)	2:1
			symbol_len = len_off/4;
			det=IR_IN_OFF;
			break;
		case SIRC: // H(4T) + L(1T)	4:1
			det=IR_IN_ON;
			symbol_len = (3*len_off)/2;
			break;
	}
	
	/* �f�[�^�[�ǎ��*/
	data_wait = 2 * symbol_len;		// �I�����o����V���{����
	noise = symbol_len /4;			// �m�C�Y�Ɣ��肷��V���{����
	for(i=0;i<IR_DATA_SIZE;i++){
		in = 0;
		for(bit=0;bit<8;bit++){
			len = ir_sens( det );
			if( len > noise && len < data_wait){
				if( len < symbol_len ){
					in = in>>1;
					in += 0;
				}else{
					in = in>>1;
					in += 128;
				}
			}else{
				in = in>>(8 - bit);
				data[i]=in;
				data_len = i * 8 + bit;
				i = IR_DATA_SIZE -1;
				bit=7;
			}
		}
		data[i]=in;
	}
	/*
	lcd_cls();						// �t������
	lcd_goto(LCD_ROW_1);
	lcd_disp_5( symbol_len );
	*/
	return(data_len);
}
