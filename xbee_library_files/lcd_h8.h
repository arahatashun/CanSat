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
元名称：LCD driver (lcd.zip -> lcd.h)
Ｃ表示：なし*/
 /***********************************************************
  *   Program:    LCD driver header
  *   Created:    27.8.99 20:26
  *   Author:     Brajer Vlado
  *                 vlado.brajer@kks.s-net.net
  *   Comments:   functions & definitions
  ************************************************************/
// LCD is connected on PORTA in 4bit mode

/*
Ｈ８版
作成者：nak@pluto.dti.ne.jp
ＵＲＬ：http://www.pluto.dti.ne.jp/~nak/h8/h8_tiny.html
取得日：2009年6月20日
元名称：HD44780 LCD driver (lcd_h8.ZIP -> lcd_h8.h)
Ｃ表示：なし*/
/*
 * HD44780 LCD driver header
 *		nak@pluto.dti.ne.jp
 *
 *  2000/11/13	for H8 Tiny(16MHz)
 *
 *  Original is ....
 *   Program:    LCD driver header
 *   Created:    27.8.99 20:26
 *   Author:     Brajer Vlado
 *   Comments:   functions & definitions
 *              .... thanks!
 */

void lcd_delay(unsigned int);
void lcd_toggle_E(void);
void lcd_cls(void);
void lcd_home(void);
void lcd_control(unsigned char , unsigned char , unsigned char );
void lcd_goto(unsigned char);
void lcd_shift(unsigned char);
void lcd_putch(char);
void lcd_putstr(const char *);
void lcd_disp_bin(unsigned char);
void lcd_disp_hex(unsigned char);
void lcd_disp_1(unsigned int);
void lcd_disp_2(unsigned int);
void lcd_disp_3(unsigned int);
void lcd_disp_5(unsigned int);
void lcd_init(void);
