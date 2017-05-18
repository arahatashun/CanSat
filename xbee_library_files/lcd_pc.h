/*********************************************************************
本ソースリストは下記からダウンロードして手を加えたものです。

	http://www.pluto.dti.ne.jp/~nak/h8/h8_tiny.html

                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/

void lcd_cls(void);
void lcd_home(void);
void lcd_control(unsigned char disonoff, unsigned char curonoff, unsigned char curblink);
void lcd_goto(unsigned char mesto);
void lcd_shift(unsigned char data);
void lcd_putch(char data);
void lcd_putstr(const char *data);
void lcd_disp_bin(unsigned char x);
void lcd_disp_hex(unsigned char i);
void lcd_disp_1(unsigned int x);
void lcd_disp_2(unsigned int x);
void lcd_disp_3(unsigned int x);
void lcd_disp_5(unsigned int x);
void lcd_init(void);
