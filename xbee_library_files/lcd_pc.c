/*********************************************************************
�{�\�[�X���X�g�͉��L����_�E�����[�h���Ď�����������̂ł��B

	http://www.pluto.dti.ne.jp/~nak/h8/h8_tiny.html

                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/

/*
����
�쐬�ҁFBrajer Vlado (vlado.brajer@kks.s-net.net)
�t�q�k�Fhttp://bray.velenje.cx/avr/lcd/lcd.html
�擾���F2007�N2��23��
�����́FLCD driver (lcd.zip -> lcd.c)
�b�\���F�Ȃ�*/
 /***********************************************************
  *   Program:    LCD driver
  *   Created:    28.8.99 21:32
  *   Author:     Brajer Vlado
  *                 vlado.brajer@kks.s-net.net
  *   Comments:   HITACHI LCD driver
  ************************************************************/

/*
�g�W��
�쐬�ҁFnak@pluto.dti.ne.jp
�t�q�k�Fhttp://www.pluto.dti.ne.jp/~nak/h8/h8_tiny.html
�擾���F2009�N6��20��
�����́FHD44780 LCD driver (lcd_h8.ZIP -> lcd_h8.c)
�b�\���F�Ȃ�*/
/*
 * HD44780 LCD driver
 *		4bit���[�h�Abusy�`�F�b�N����
 *
 *  Original is ....
 *   Program:    LCD driver
 *   Created:    28.8.99 21:32
 *   Author:     Brajer Vlado
 *   Comments:   HITACHI LCD driver
 */

//(8)DDRAM�A�h���X�Z�b�g
#define		LCD_ROW_1		0x00	//�P�s�ڐ擪�A�h���X
#define		LCD_ROW_2		0x40	//�Q�s�ڐ擪�A�h���X
#define		LCD_ROW_3		0x14	//�P�s�ڐ擪�A�h���X
#define		LCD_ROW_4		0x54	//�Q�s�ڐ擪�A�h���X

#include <stdio.h>

/*--------------------------------------------------
 *  Clear display
 *--------------------------------------------------*/
void lcd_cls(void)
{
	int i;
	putchar('\n');
	for(i=0;i<20;i++){
		printf("-");
	}
	putchar('\n');
}

/*--------------------------------------------------
 *  Home position
 *--------------------------------------------------*/
void lcd_home(void)
{
	lcd_cls();
}

/*--------------------------------------------------
 *  LCD & Cursor control (Blinking, ON/OFF,...)
 *--------------------------------------------------*/
void lcd_control(unsigned char disonoff, unsigned char curonoff, unsigned char curblink)
{
}

/*--------------------------------------------------
 *  Goto position   (0x00�` - line1)
 *                  (0x40�` - line2)
 *--------------------------------------------------*/
void lcd_goto(unsigned char mesto)
{
	putchar('\n');
}

/*--------------------------------------------------
 *  Cursor shift on LCD
 *--------------------------------------------------*/
void lcd_shift(unsigned char data)
{
	putchar('\n');
}

/*--------------------------------------------------
 *  Put character on LCD
 *--------------------------------------------------*/
void lcd_putch(char data)
{
	putchar( data );
}

/*--------------------------------------------------
 *  Display null terminated string
 *--------------------------------------------------*/
void lcd_putstr(const char *data)
{
	unsigned char i=0;
	while(data[i] != 0 && i < 255 ) {
	    lcd_putch(data[i]);
	    i++;
	}
}

/*-----------------27.5.99 20:29--------------------
 *  Display 8bit bin value
 *--------------------------------------------------*/
void lcd_disp_bin(unsigned char x)
{
	unsigned char i;
	for (i=128;i>0;i>>=1){
		if ((x&i)==0){
			lcd_putch('0');
		}else{
			lcd_putch('1');
		}
	}
}

/*-----------------1.6.99 16:39---------------------
 *  Display unsigned char in hex
 *--------------------------------------------------*/
void lcd_disp_hex(unsigned char i)
{
    unsigned char hi,lo;
    hi=i&0xF0;               // High nibble
    hi=hi>>4;
    hi=hi+'0';
    if (hi>'9'){
        hi=hi+7;
	}
    lo=(i&0x0F)+'0';         // Low nibble
    if (lo>'9'){
        lo=lo+7;
	}
    lcd_putch((char)hi);
    lcd_putch((char)lo);
}

/* --------------------------------------------------
              http://www.geocities.jp/bokunimowakaru/
 * --------------------------------------------------*/
void lcd_disp_1(unsigned int x)
{
	if (x<10){
		lcd_putch((char)(x+0x30));
	}else if (x<16){
		lcd_putch((char)(x-10+'A'));
	}else{
		lcd_putstr("X");
	}
}

/*-----------------28.08.99 23:00-------------------
 *   Display 0-99
 * --------------------------------------------------*/
void lcd_disp_2(unsigned int x)
{
	char s[3];
	unsigned int y;
	if (x<100){
		y=x/10; s[0]=(char)(y+0x30); x-=(y*10);
		s[1]=(char)(x+0x30);
		s[2]='\0';
		if( s[0]=='0' ){
			s[0]=' ';
		}
		lcd_putstr( s );
	}else{
		lcd_putstr("XX");
	}
}

/*-----------------27.5.99 20:29--------------------
 *  Display 0-999 unsigned char or int (8 or 16bit)
 *--------------------------------------------------*/
void lcd_disp_3(unsigned int x)
{
	char s[4];
	unsigned int y;
	if (x<1000){
		y=x/100; s[0]=(char)(y+0x30); x-=(y*100);
		y=x/10;  s[1]=(char)(y+0x30); x-= (y*10);
		s[2]=(char)(x+0x30);
		s[3]='\0';
		if( s[0]=='0' ){
			s[0]=' ';
			if( s[1]=='0' ){
				s[1]=' ';
			}
		}
		lcd_putstr( s );
	}else{
		lcd_putstr("XXX");
	}
}

/*-----------------27.5.99 20:30--------------------
 *  Display integer 0-65535 (16bit)
 *--------------------------------------------------*/
void lcd_disp_5(unsigned int x)
{
	char s[6];
	unsigned int y;
	if (x<=65535){
		y=x/10000; s[0]=(char)(y+0x30); x-=(y*10000);
		y=x/1000;  s[1]=(char)(y+0x30); x-= (y*1000);
		y=x/100;   s[2]=(char)(y+0x30); x-=  (y*100);
		y=x/10;    s[3]=(char)(y+0x30); x-=   (y*10);
		s[4]=(char)(x+0x30);
		s[5]='\0';
		if( s[0]=='0' ){
			s[0]=' ';
			if( s[1]=='0' ){
				s[1]=' ';
				if( s[2]=='0' ){
					s[2]=' ';
					if( s[3]=='0' ){
						s[3]=' ';
					}
				}
			}
		}
		lcd_putstr( s );
	}else{
		lcd_putstr("XXXXX");
	}
}

/*--------------------------------------------------
 *  Display initialization
 *--------------------------------------------------*/
void lcd_init(void)
{
	lcd_cls();				// �\���N���A
}
