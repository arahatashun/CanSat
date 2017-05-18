/*********************************************************************
本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

unsigned char 16進数をASCIIに変換する。

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
