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
�����́FLCD driver (lcd.zip -> lcd.h)
�b�\���F�Ȃ�*/
 /***********************************************************
  *   Program:    LCD driver header
  *   Created:    27.8.99 20:26
  *   Author:     Brajer Vlado
  *                 vlado.brajer@kks.s-net.net
  *   Comments:   functions & definitions
  ************************************************************/
// LCD is connected on PORTA in 4bit mode

/*
�g�W��
�쐬�ҁFnak@pluto.dti.ne.jp
�t�q�k�Fhttp://www.pluto.dti.ne.jp/~nak/h8/h8_tiny.html
�擾���F2009�N6��20��
�����́FHD44780 LCD driver (lcd_h8.ZIP -> lcd_h8.h)
�b�\���F�Ȃ�*/
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
