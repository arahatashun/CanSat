/*********************************************************************
本ソースリストは下記からダウンロードして手を加えたものです。

	http://www.pluto.dti.ne.jp/~nak/h8/h8_tiny.html

                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/

/*
原作
作成者：Brajer Vlado (vlado.brajer@kks.s-net.net)
ＵＲＬ：http://bray.velenje.cx/avr/lcd/lcd.html
取得日：2007年2月23日
元名称：LCD driver (lcd.zip -> lcd.c)
Ｃ表示：なし*/
 /***********************************************************
  *   Program:    LCD driver
  *   Created:    28.8.99 21:32
  *   Author:     Brajer Vlado
  *                 vlado.brajer@kks.s-net.net
  *   Comments:   HITACHI LCD driver
  ************************************************************/

/*
Ｈ８版
作成者：nak@pluto.dti.ne.jp
ＵＲＬ：http://www.pluto.dti.ne.jp/~nak/h8/h8_tiny.html
取得日：2009年6月20日
元名称：HD44780 LCD driver (lcd_h8.ZIP -> lcd_h8.c)
Ｃ表示：なし*/
/*
 * HD44780 LCD driver
 *		4bitモード、busyチェック無し
 *
 *  Original is ....
 *   Program:    LCD driver
 *   Created:    28.8.99 21:32
 *   Author:     Brajer Vlado
 *   Comments:   HITACHI LCD driver
 */

#ifndef TARGET_H8_H
	#define TARGET_H8_H
	#include <3694.h>
#endif
#ifndef LCD_H8_H
	#define LCD_H8_H
	#include "lcd_h8.h"
#endif

#define		LCD_PORT		IO.PDR5.BYTE				// P5x
#define		LCD_OUT_MASK	0xD0						// P56-P57,P54 masked
#define		LCD_PORT_BM		(LCD_PORT & LCD_OUT_MASK)	// LCD
#define		LCD_PCR_PORT	IO.PCR5     				// P5ポート出力方向
//	xxbx bbbb
//	|||| ||||______	P50: D4		out
//	|||| |||_______	P51: D5		out
//	|||| ||________	P52: D6		out
//	|||| |_________ P53: D7		out
//	||||___________ P54: E		out	(旧RW)
//	|||____________ P55: RS		out
//	||_____________ P56: 空き	out	(旧E)
//	|______________ P57: 空き	---

#define		HD44780_BIT_RS      IO.PDR5.BIT.B5
#define		HD44780_BIT_E       IO.PDR5.BIT.B4

// インストラクション
//(1)表示クリア
#define		LCD_INSTR_CLS_Hi		0x00
#define		LCD_INSTR_CLS_Lo		0x01
//	xx00 0000
//	xx00 0001
//	  || ||||______	Data=0x01
//	  ||___________ RW = 0
//	  |____________ RS = 0

//(2)カーソルホーム
#define		LCD_INSTR_HOME_Hi		0x00
#define		LCD_INSTR_HOME_Lo		0x02
//	xx00 0000
//	xx00 001x
//	  || ||||______	Data=0x02
//	  ||___________ RW = 0
//	  |____________ RS = 0

//(3)エントリーモードセット
#define		LCD_INSTR_ENT_Hi		0x00	// 
#define		LCD_INSTR_ENT_Lo		0x06	// 表示シフト無効,アドレス+1＆カーソル右
//	xx00 0000
//	xx00 01IS
//	  ||   ||______	S  1:表示シフト有効 / 0:表示シフト無効
//	  ||   |_______ I  1:アドレス+1＆カーソル右 / 0:アドレス-1＆カーソル左
//	  ||___________ RW = 0
//	  |____________ RS = 0

//(4)表示ON/OFFコントロール
#define		LCD_INSTR_INICON_Hi		0x00	// Driver Init
#define		LCD_INSTR_INICON_Lo		0x0F	// Driver Init
#define		LCD_INSTR_CON_Hi		0x00
#define		LCD_INSTR_CON_Lo		0x08	// Data=0x08(+0x04/+0x02/+0x01)
//	xx00 0000
//	xx00 1DCB
//	  ||  |||______	B  Blink	1:on / 0:off
//	  ||  ||_______ C  Cursor	1:on / 0:off
//	  ||  |________ D  Display	1:on / 0:off
//	  ||___________ RW = 0
//	  |____________ RS = 0

//(5)カーソル/表示シフト
#define		LCD_INSTR_SIFT_Hi	0x01	// 
#define		LCD_INSTR_SIFT_Lo	0x00	// カーソルシフト
//	xx00 0001
//	xx00 RSxx
//	  || ||________	R/L  1:右シフト(+1) / 0:左シフト(-1)
//	  || |_________ S/C  0:カーソルシフトに固定
//	  ||___________ RW = 0
//	  |____________ RS = 0

//(6)ファンクションセット
#define		LCD_INSTR_INIT8		0x03	// 8bitモード
#define		LCD_INSTR_INIT4		0x02	// 8->4bitモード変更
#define		LCD_INSTR_FC_Hi		0x02	// 4bitモード
#define		LCD_INSTR_FC_Lo		0x08	// 1/16duty,5x7dot
//	xx00 001D
//	        |______ DL  1:8bitモード / 0:4bitモード
//	xx00 NFxx
//	  || ||||______	無効
//	  || ||________ F  1:5x10dot / 0:5x7dot    <=== M1632(N=1)では無効
//	  || |_________ N  1:1/16duty / 0:1/8duty  <=== M1632では"1"固定
//	  ||___________ RW = 0
//	  |____________ RS = 0

//(8)DDRAMアドレスセット
#define		LCD_ROW_1		0x00	//１行目先頭アドレス
#define		LCD_ROW_2		0x40	//２行目先頭アドレス
#define		LCD_ROW_3		0x14	//３行目先頭アドレス
#define		LCD_ROW_4		0x54	//４行目先頭アドレス

#define		LCD_INSTR_GOTO_Hi		0x08
#define		LCD_INSTR_GOTO_Lo		0x00
//	xx00 1aaa
//	xx00 aaaa
//	  || ||||______	DDRAMアドレス
//	  ||___________ RW = 0
//	  |____________ RS = 0

//(10)CGRAM/DDRAMへのデータ書き込み
#define		LCD_INSTR_PUT_Hi		0x20
#define		LCD_INSTR_PUT_Lo		0x20
//	xx10 dddd
//	xx10 dddd
//	  || ||||______	DDRAMアドレス
//	  ||___________ RW = 0
//	  |____________ RS = 1

/*--------------------------------------------------
 *  delay
 *--------------------------------------------------*/
#define     WAIT_CNT 	10	// 16MHz -> 62.5ns/cyc
//  optimize(o3)
//  data=0   -> 3.5us
//  data=1   -> 10.5us
//  data=5   -> 35us
//	data=10  -> 70us
//  data=100 -> 0.7ms
//  data=215 -> 1.5ms
void lcd_delay(unsigned int data)
{
	unsigned int loop;
	while(data != 0){
		for(loop=0; loop<WAIT_CNT; loop++);
		data--; 
	}
}

/*--------------------------------------------------
 *  Clock(E)
 *--------------------------------------------------*/
void lcd_toggle_E(void)
{
	// tAS MIN=140ns
	HD44780_BIT_E = 1;		// E=1
	// PW_EH MIN=450ns
	HD44780_BIT_E = 0;		// E=0
	// tAH MIN=10ns
}

/*--------------------------------------------------
 *  Clear display
 *--------------------------------------------------*/
void lcd_cls(void)
{
	unsigned char	uB;
	
	uB = LCD_INSTR_CLS_Hi;		// 1st
	uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	uB = LCD_INSTR_CLS_Lo;		// 2nd
	uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	lcd_delay(250);				//wait 1.6ms
}

/*--------------------------------------------------
 *  Home position
 *--------------------------------------------------*/
void lcd_home(void)
{
	unsigned char	uB;
	
	uB = LCD_INSTR_HOME_Hi;		// 1st
	uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	uB = LCD_INSTR_HOME_Lo;		// 2nd
	uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	lcd_delay(250);				//wait 1.6ms
}

/*--------------------------------------------------
 *  LCD & Cursor control (Blinking, ON/OFF,...)
 *--------------------------------------------------*/
void lcd_control(unsigned char disonoff, unsigned char curonoff, unsigned char curblink)
{
	unsigned char	uB;
	// 1st
	uB = LCD_INSTR_CON_Hi;
	uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	// 2nd
	uB = LCD_INSTR_CON_Lo;
	if (disonoff==1) uB |= 0x04;	//D 1:表示on/0:表示off
	if (curonoff==1) uB |= 0x02;	//C	1:カーソルon/0:カーソルoff
	if (curblink==1) uB |= 0x01;	//B	1:ブリンクon/ブリンクoff
	uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	lcd_delay(10);	//wait 40us
}

/*--------------------------------------------------
 *  Goto position   (0x00～ - line1)
 *                  (0x40～ - line2)
 *--------------------------------------------------*/
void lcd_goto(unsigned char mesto)
{
	unsigned char	temp, uB;
	
	temp = mesto>>4;			//上位アドレス
	uB = LCD_INSTR_GOTO_Hi;		// 1st
	uB |= temp;	
	uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	temp = mesto & 0x0F;		//下位アドレス
	uB = LCD_INSTR_GOTO_Lo;		// 2nd
	uB |= temp;
	uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	lcd_delay(10);				//wait 40us
}

/*--------------------------------------------------
 *  Cursor shift on LCD
 *--------------------------------------------------*/
void lcd_shift(unsigned char data)
{
	unsigned char	uB;
	
	uB = LCD_INSTR_SIFT_Hi;		// 1st
	uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	uB = LCD_INSTR_SIFT_Lo;		// 2nd
	if( data ) uB |= 0x04;		// 右シフト
	uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	lcd_delay(10);				//wait 40us
}

/*--------------------------------------------------
 *  Put character on LCD
 *--------------------------------------------------*/
void lcd_putch(char data)
{
	unsigned char	temp, uB;
	
	temp = data>>4;				//上位データ
	uB = LCD_INSTR_PUT_Hi;		// 1st
    uB |= temp;
    uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	temp = data & 0x0F;			// 下位データ
	uB = LCD_INSTR_PUT_Lo;		// 2nd
	uB |= temp;
	uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	lcd_delay(10);				//wait 40us
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
	unsigned int y;
	if (x<100){
		y=x/10;lcd_putch((char)(y+0x30));x-=(y*10);
		lcd_putch((char)(x+0x30));
	}else{
		lcd_putstr("XX");
	}
}

/*-----------------27.5.99 20:29--------------------
 *  Display 0-999 unsigned char or int (8 or 16bit)
 *--------------------------------------------------*/
void lcd_disp_3(unsigned int x)
{
	unsigned int y;
	if (x<1000){
		y=x/100;lcd_putch((char)(y+0x30));x-=(y*100);
		y=x/10;lcd_putch((char)(y+0x30));x-=(y*10);
		lcd_putch((char)(x+0x30));
	}else{
		lcd_putstr("XXX");
	}
}

/*-----------------27.5.99 20:30--------------------
 *  Display integer 0-65535 (16bit)
 *--------------------------------------------------*/
void lcd_disp_5(unsigned int x)
{
	unsigned int y;
	if (x<=65535){
		y=x/10000;lcd_putch((char)(y+0x30));x-=(y*10000);
		y=x/1000;lcd_putch((char)(y+0x30));x-=(y*1000);
		y=x/100;lcd_putch((char)(y+0x30));x-=(y*100);
		y=x/10;lcd_putch((char)(y+0x30));x-=(y*10);
		lcd_putch((char)(x+0x30));
	}else{
		lcd_putstr("XXXXX");
	}
}

/*--------------------------------------------------
 *  Display initialization
 *--------------------------------------------------*/
void lcd_init(void)
{
	unsigned char	uB;
	
  	LCD_PCR_PORT |= 0x7F;   // out port
  	// x111 1111
  	// |___________ 空き

	lcd_delay(2500);		// 15ms以上wait
	// デフォルト設定 8bitモード
	uB = LCD_INSTR_INIT8;
	uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	lcd_delay(750);			// 5ms以上wait
	uB = LCD_INSTR_INIT8;
	uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	lcd_delay(20);			// 100us以上wait
	// デフォルト設定 8bit->4bitモード
	uB = LCD_INSTR_INIT4;
	uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	lcd_delay(10);			//wait 40us
	// ファンクションセット
	uB = LCD_INSTR_FC_Hi;
	uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	uB = LCD_INSTR_FC_Lo;
	uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	lcd_delay(10);			//wait 40us
	// 表示on/off初期化
	uB = LCD_INSTR_INICON_Hi;
	uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	uB = LCD_INSTR_INICON_Lo;
	uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	lcd_delay(10);			//wait 40us
	// エントリーモードセット
	uB = LCD_INSTR_ENT_Hi;
	uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	uB = LCD_INSTR_ENT_Lo;
	uB |= LCD_PORT_BM;
	LCD_PORT = uB;
	lcd_toggle_E();
	lcd_delay(10);			//wait 40us
	lcd_cls();				// 表示クリア
}
