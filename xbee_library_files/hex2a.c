/*********************************************************************
�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

unsigned char 16�i����ASCII�ɕϊ�����B

                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/

void hex2a( const unsigned char i , char *s ){
	unsigned char c;
    c = i&0xF0;
    c = c>>4;
    if ( c > 9 ) s[0] = '0' + c + 7;
    		else s[0] = '0' + c;
    c = (i&0x0F);
    if ( c > 9 ) s[1] = '0' + c + 7;
    		else s[1] = '0' + c;
}
