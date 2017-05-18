/*********************************************************************
本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

ZB Coord API for XBee

This XBee library drives Digi XBee ZB Modules in API Operation mode.
Most commands include remote communication's are supported by the
functions of this library. You can control XBee devices through the
API software modules in this, without using any AT commands.

							   Copyright (c) 2010-2014 Wataru KUNINO
							   http://www.geocities.jp/bokunimowakaru/
*********************************************************************/

//	for ARM mbed Compiler

//		#define ARM_MBED
//		#include <mbed.h>
//		RawSerial _xbee_serial(SERIAL_TX, SERIAL_RX);	// for STM32 Nucleo

//	Please set serial ports in conformity to your microcomputer board.
//	上記の括弧内をお手持ちのマイコンボードのシリアルピン名に書き換えてください。

/*
既知のバグ
	[PC用/H8用/Arduino共通]
	・xbee_force／ATISでERRORもしくは正しい値が得られない場合がある。
	　一度、xbee_gpo／ATP104などで子機のGPIOなどに出力すると治る。
	　　(XBeeモジュールの不具合と思われる)
	・XBee Smart Plug/XBee Sensor等でネットワーク参加直後、
	　正しい値を応答しない場合がある。上記と似た症状かもしれない。
	　一度、XBee Smart PlugのACコンセントを抜いて挿しなおせば治る。
	　　(Smart Plug側のファームウェア「XBP24-ZB_2264」の問題と思われる)
	・Ver 1.90でLITEモードの仕様を変更。（濱崎氏によるBeeBee Lite）

	★XBee Wi-FiでのRAT実行時にXBeeから応答のパケットが無い場合がある。
	  IP_PINGは通る。
	  再現方法、動作中にXBeeをリセットし、その後にArduinoをリセット。
	  XBeeモジュールをリセットすると治る。
	  Arduino版で発覚。PC版は未確認。

	[パソコン用]
	・おかげさまで特記事項なし。安定して動作します。

	[Arduino用]
	・標準SDライブラリ使用時にメモリ不足になる場合があります。
	　そのような場合はxbee_liteをincludeし、LITEモードにしてください。

	[ARM mbed用]
	・試作品です。バグ多数かもしれません。
	・STマイクロ製 Nucleo Board STM32F401で動作確認しています。
	・XBeeとの接続にはArduino用Wireless SD Shieldを使用します。
	　※Nucleoボードの裏側のジャンパー抵抗の変更が必要です。
				「SB14」をオープンにします（抵抗を取り外します。）
				「SB62」をショートします。（半田で接続します。）
				「SB63」をショートします。（半田で接続します。）
	・液晶はDF ROBOT製 LCD Keypad Shieldで動作確認しています。

	[H8マイコン用]
	・Ver1.52以降は動作確認をしておらず、動作しない恐れがあります。
	　お使いになる方は連絡ください。動作確認版を個別リリースいたします。

制限事項
	・ソースはPC/H8/Arduino/ARM mbed用で共通ですが拡張子が異なります。
	　PC/H8「xbee.c」をArduino用は「xbee_arduino」フォルダ内のライブラリ
	　「xbee.h」をincludeしてください。
	・xbee_forceで指示を出してxbee_rx_callで待ち受ける用法を推奨します。
	・受信パケットの最大APIフレーム長は255Bytes等に制限されます。(64KB)
	・複数のXBeeデバイスの選択はアプリでIEEEアドレスを指定してください。
	・ショートアドレスは使っていません(IEEEアドレスで指定してください)
	・温度測定を行う場合は内部発熱分を補正(減算)する必要があります。
	　Digi純正のWall RouterやSmart Plugの場合は内部発熱の４℃を考慮して
	　測定値から減算必要があります。
	・送信者アドレスを得る関数xbee_fromは、xbee_rx_callで受信した場合に
	　正しく得られない場合があります。(受信キャッシュが保持されている時)
	　xbee_rx_callに渡したxbee_result.FROM[8]から送信者を得てください。

アプリ向け提供関数

	byte xbee_myaddress( byte *address );
	void xbee_address( const byte *address );
	byte xbee_atd( const byte *address );
	byte xbee_ratd(const byte *dev_address, const byte *set_address );
	byte xbee_ratd_myaddress(const byte *address);
	void xbee_from( byte *address );
	byte xbee_at(const char *in);
	byte xbee_rat(const byte *address, const char *in);
	byte xbee_rat_force(const byte *address, const char *in);
	byte xbee_uart(const byte *address, const char *in);
	byte xbee_bytes(const byte *address, const byte *in, byte len);
	byte xbee_atvr( void );
	byte xbee_atai( void );
	byte xbee_atcb( byte cb );
	unsigned short xbee_atop(byte *pan_id);
	byte xbee_atee_on(const char *key );
	byte xbee_atee_off( void );
	byte xbee_atnc( void );
	byte xbee_ratnc( const byte *address );
	byte xbee_atnj( const byte timeout );
	byte xbee_ratnj( const byte *address, const byte timeout );
	byte xbee_ping( const byte *address );
	int xbee_batt(const byte *address );
	byte xbee_batt_force(const byte *address );
	byte xbee_gpio_config(const byte *address, const byte port, const enum xbee_port_type type ); //(PC/H8)
	byte xbee_gpio_config(const byte *address, const byte port, const byte type );			//(arduino)
	byte xbee_gpio_init( const byte *address );
	byte xbee_gpo( const byte *address, const byte port,const byte out );
	byte xbee_gpi(const byte *address, const byte port);									//(EASY)
	unsigned int xbee_adc(const byte *address, const byte port);							//(EASY)
	byte xbee_force( const byte *address );
	float xbee_sensor_result( XBEE_RESULT *xbee_result, const enum xbee_sensor_type type ); //(PC/H8)
	float xbee_sensor_result( XBEE_RESULT *xbee_result, const byte type);					//(arduino)
	byte xbee_rx_call( XBEE_RESULT *xbee_result );
	void xbee_clear_cache(void);
	xbee_sensor(const byte *address, const enum xbee_sensor_type type );					//(EASY)
	byte xbee_init( const byte port );
	byte xbee_reset( void );
	byte xbee_end_device(const byte *address, byte sp, byte ir, const byte pin);
	byte xbee_i2c_read(const byte *address, byte adr, byte *rx, byte len);
	byte xbee_i2c_write(const byte *address, byte adr, byte *rx, byte len);
	byte xbee_delay(unsigned int ms);
	void lcd_disp( char *s );

主要な変更履歴
	2010/08/15	0.10	- 初回の公開版をリリース 基本動作部分の実験用
	2011/08/14	0.30	- ライブラリ化のためにAPI関数の内容を全面見直し
	2012/04/07	0.80	- 総合試験の実施とバグ修正、仕様見直しなど
	2012/04/22	0.91	- パソコン(cygwin)への移植
	2012/07/15	1.00	- 基本機能(sample1～8)のデバッグ完了。正式版
	2012/08/25	1.15	- Arduino用へ移植
						- enum xbee_port_typeのADCをAINに変更
						- 起動時にシリアル速度が設定されない不具合を修正
	2012/11/25	1.20	- 電池電圧確認用コマンドを非同期型に変更
						  xbee_batt⇒xbee_batt_force。応答はxbee_rx_call
	2012/12/09	1.30	- 液晶なしオプション(#define LCD_H)の追加
						- Liteモード(#define LITE)の追加。ARDUINO版のみ
						- PC版シリアルポートをcom1～com10に拡張
						- グローバル変数アドレスのADR_MYとSADR_MYを廃止
	2012/12/16	1.50	- Arduino版のライブラリ化
						- コマンド応答待ち時に他のパケットをキャッシュ
						  (#define CACHE_RES 10でキャッシュが有効になる)
	2012/12/17	1.51	- リモートATの応答確認にAT値からMODE値に変更
						- H8のバグ対応(lcd_h8.cの2重呼び出し対策等)
	2012/12/18	1.52	- [重要] 関数lcd()をlcd_disp()に変更。
						- Arduino版ライブラリ側ハードLCD,LED,SWの無効化
	2012/12/22	1.53	- [重要] atnjとratnjの戻り値を失敗時255に変更
						- ATコマンドを送信する関数を追加
	2012/12/23	1.54	- 1.53のエンバグの修正(テスト用printfの残留)
	2012/12/30	1.55	- Arduino LEONARDOへの対応
						- Arduino標準LCDの配線変更(DF ROBOT互換に)
	2013/01/12	1.56	- xbee_result.GPI[0]とGPI[1]の戻り値を入れ替え
	2013/01/14	1.57	- xbee_gpio_configでアドレス取得失敗時のバグ修正
	2013/01/20	1.58	- xbee状態取得 xbee_ataiとxbee_atopコマンド追加
	2013/04/03	1.60	- xbee_initによるXBeeリセット時の応答の廃棄処理
						- エラー出力を stderrに変更PC用(のみ)
						- xbee_result.GPI.PORT.Dnのエンバグ(1.56～)修正
	2013/04/08	1.71	- [重要] Arduino xbee_rx_callを構造体受取に変更
						- [重要] Arduino xbee_sensor_resultの引数の同上
						- [重要] xbee_rx_callパケット選択受信機能を削除
						- [重要] PC/H8 xbee_result.ADCをADCINに変更
						- xbee_atに16進数のテキスト引数の処理のバグ修正
						- xbee_atnjでjoin後にatnj=5を設定(5秒Join有効)
						- PC/H8/Arduinoでライブラリを共通ソースに統合
	2013/04/21	1.72	- コマンド応答をxbee_rx_callで得るxbee_rat_force
						- テキストをシリアル送信するxbee_uartの追加
						- ZigBeeデバイスタイプを取得するxbee_atvrの追加
						- ネットワーク参加状況を確認するxbee_ataiの追加
						- コミッショニング操作を行うxbee_atcbの追加
						- ネットワークIDを取得するxbee_atopの追加
						- 暗号化xbee_atee_onと解除xbee_atee_offの追加
	2013/04/30	1.73	- PC版＆Arduino版リリース
						- 受信キャッシュオーバー時パケット廃棄(仕様変更)
	2013/05/09	1.74	- 初期化時にAPモード1の設定を追加
						- #define LCD_H有効時以外のログ呼び出し停止
						- 子機に本機アドレスを設定するxbee_set_myaddress
	2013/05/10	1.75	- End Deviceにスリープを設定するxbee_end_device
	2013/05/18	1.76	- [重要] 戻り値を送信パケット番号PACKET_IDに変更
						  xbee_gpo,同_force,同_batt,同_rat_force,同_uart
						- [重要] Arduino xbee_resultにxbee_result.ID追加
	2013/05/25	1.77	- XBee Wi-Fi XBee IPによるリモートAT制御に対応
	2013/06/30	1.78	- 全マスクGPIO入力時xbee_result.GPI.BYTEを0xFFFF
						- GPIO出力(xbee_gpo)の利用可能ポート範囲を拡大
	2013/08/28	1.79	- XBee Sensorのデバイス名(define)の追加
						- xbee_end_deviceの親機のSP値を28秒固定に変更
	2013/09/28	1.80	- xbee_end_device Router時の自動送信設定を可能に
						- xbee_resultのGPINに電池電圧ADC[0]にmV値を応答
						- xbee_atnjでjoin後のatnj=5設定をatnj=0に変更
						- リモートAT応答の送信元が取得できないバグ修正
						- XBee Wi-Fi xbee_gpoを送信時の応答バグ修正
	2013/10/14	1.81	- Arduino版XBee Wi-Fi対応。XBee ZBは未テスト
						- xbee_atd追加(End Device時に宛先アドレスを保存)
						- xbee_ratd追加(親機アドレス以外も設定可能に)
						- xbee_set_myaddress⇒xbee_ratd_myaddress名変更
						- XBee Wi-Fi 3バイト以下UART受信不具合の修正
						- XBee Wi-Fi ATDDで不適切な値を応答するバグ対策
						- XBee Wi-Fiのフレーム受信でメモリーリーク対策
	2014/01/14	1.82	- ATmega32U4が使われている時はLEONARDOを自動定義
						- シリアルポート異常時のSegFault不具合の修正
						- 日時付エラーログ出力(PC用XBEE_ERROR_TIME定義)
	2014/02/17	1.83	- XBee Wi-Fi サンプルの各種不具合修正
						- XBee Wi-Fi S2B対応(机上検討による実装)
						- バイナリデータを子機UARTへ送信するxbee_bytes
						- I2Cインタフェースを読み取るxbee_i2c_readの追加
						- I2Cインタフェースで書き込むxbee_i2c_write追加
						- 受信キャッシュをクリアするxbee_clear_cache追加
						- 特定者向けリリース(I2C接続LCD,XBee Wi-Fi S2B)
	2014/02/23	1.84	- 無駄使いメモリの修正、表示の調整、累積バグ修正
						- ATコマンド解析ツールxbee_test.cのhelpコマンド
						- README内のバージョン齟齬に関する修正
	2014/03/01	1.85	- XBee WiFi S6B Ver.20xx対応(Ver.10xxと自動切替)
						　対応モジュール　XBee WiFi S6	Ver.102D
						　　　　　　　　　XBee WiFi S6B Ver.2021
	2014/03/17	1.86	- CQ出版様向けサンプルソフトウェアの正式リリース
	2014/06/21	1.87	- ARM mbed対応(試作品)
	2014/08/02	1.88	- シリアルCOMポート拡張(ポート番号1～64に対応)
						- Arduino xbee_initでリトライ回数指定に。0で永久
						- xbee_initに戻り値。異常時0。正常時リトライ数
						- 受信しながら時間待ちするxbee_delay関数の追加
						- 宛先ショートアドレス指定関数xbee_short_address
						- ZigBee ZCL使用時(PC用)に関するバグ修正
	2014/08/24	1.89	- 内部関数xbee_resetの公開(ヘッダファイルへ追加)
						- xbee_init 失敗時の戻り値の不具合を修正
						- PC版 GCC Ver 4.7対応,最適化オプション-O1の付与
						- 戻り値が不定になる内部関数の不具合修正
	2014/09/15	1.90	- 有志によるライブラリBeeBee Liteの一部機能採用
						　・試験ツールxbee_agingを通過しない項目の調整
						　・効果はArduino上でSD使用時のメモリ確保など
						- xbee_bytesで規定サイズを超えた場合のリーク対策
						- 下位マスク0x00のGPIN受信時にエラー出力バグ修正
	2014/10/25	1.91	- CQ出版 ZigBee Arduinoプログラム全集 第2版用
						- ARM mbedでのシリアル受信時のロックアップ対策
						- ARM mbedを用いた時のシリアル送信タイミング調整
						- xbee_end_deviceでRouterへ送信時にIR値を設定
						　(従来はエラー応答だったが、設定後に正常応答)
	2014/10/31	1.92	- ARM mbed対応

*********************************************************************/
/*
本ライブラリのバージョン
*/
#ifndef VERSION

	#define 	VERSION "1.92"		// 1.XX 4バイト形式 XXは半角文字

#endif
/*
参考文献

	(1) Digi International Inc.
		XBee ZB RF Modules 90000976_D 8/18/2009
		XBee Wi-Fi RF Module 90002124_F 2011
	(2) Massimo Banzi著 船田功訳
		Arduinoをはじめよう (オライリージャパン)
	(3) BestTechnology CO.,LTD.
		H8/3664F I/O address definition Release 3.5 2005-11-08 (3694.h)
	(4) 粕谷友章
		PC-UNIX関連ドキュメント「シリアルポートプログラミング」
		www006.upp.so-net.ne.jp/kasuya/linux/serial_io_programing.html
	(5) JM Project Linux Programmer's Manual
		Linux man-pages プロジェクト リリース 3.52 (GPL V2)
		linuxjm.sourceforge.jp/html/LDP_man-pages/man2/select.2.html
	(6) mbed Handbook
		mbed.org/handbook/Homepage

ライセンスについて

	本ソースコードはライセンスフリーですが、参考文献の情報については
	それぞれの権利者にお問い合わせください。

注意点
	本ソフトの利用による損害について当方は一切の補償をいたしません。
	全て自己責任で利用ください。
*/

/*********************************************************************
ライブラリ使用説明書

ハードウェア

	PC用 以下のハードが必要です。
			・USBを搭載したIBM PC/AT互換機(通常のWindows PC)
			・XBee USBエクスプローラ、XBee-Fly USB または 純正XBIB-U-DEVなど
			　(http://www.geocities.jp/bokunimowakaru/diy/xbee/xbee-usb.html)
			・XBee Series 2 (ZB) モジュール

	Arduino 以下のハードが必要です。
			・Arduinoマイコンボード Arduino UNO Arduino Leonardo等
			・XBee Shield (Arduino Wireless SD Shieldなど)
			・XBee Series 2 (ZB) モジュール
			・LCDキャラクタディスプレイモジュール[20×4行] SC2004CS-B

			　キャラクタ液晶は以下のように接続する

			　						  rs rw en d0 d1 d2 d3
			　// LiquidCrystal xbee_ardlcd( 8, 7, 6, 5, 4, 3, 2)
			　
	ARM mbed用
			・STマイクロ製 Nucleo Board STM32F401で動作確認しています。
			・XBeeとの接続にはArduino用Wireless SD Shieldを使用します。
			　※Nucleoボードの裏側のジャンパー抵抗の変更が必要です。
					「SB14」をオープンにします（抵抗を取り外します。）
					「SB62」をショートします。（半田で接続します。）
					「SB63」をショートします。（半田で接続します。）
			・液晶を使用したサンプルはDF ROBOT製 LCD Keypad Shieldで
			　動作確認しています。

	H3694用  秋月電子通商で販売されている以下のハードウェアが必要です。
			・AKI-H8/3694F(QFP) タイニーマイコンキット
			・Ｈ８タイニーＩ／Ｏ（アイ・オー）ボード
			・小型スライドスイッチ 1回路2接点 SS12D01G4
			・LCDキャラクタディスプレイモジュール[20×4行] SC2004CS-B
			・XBeeモジュールをRS-232Cに接続する基板(純正XBIB-R-DEVなど)
			　(http://www.geocities.jp/bokunimowakaru/pict/xbee-cord_sch.gif)
			・RS-232Cケーブル
			・XBee Series 2 (ZB) モジュール

			　キャラクタ液晶はH8/3694のCN1の14～19ピンの各出力と電源、
			　GNDを液晶モジュールのDB4～7およびE、RSの各入力、および
			　電源に接続して製作します(下図参照)。

					AKI-H8　　　　　　液晶モジュール
					(CN1)　　　　　　 (HD44780搭載)
					━━┓　　　　　　┏━━━━━┓　　　　┯ 5V
					　　┃P50　　　　 ┃　　　　　┃　　　　｜
					　14┠──────┨DB4　　Vdd┠───●┘
					　　┃P51　　　　 ┃　　　　　┃　　　｜
					　15┠──────┨DB5 　　　┃　　　＜
					　　┃P52　　　　 ┃　　　　　┃　┌→＞
					　16┠──────┨DB6　　VLC┠─┘　＜ 10k
					　　┃P53　　　　 ┃　　　　　┃　　　｜
					　17┠──────┨DB7　DB0-3┠─┐　｜
					　　┃P54　　　　 ┃　　　　　┃　｜　｜
					　18┠──────┨E　　　R/W┠─●　｜
					　　┃P55　　　　 ┃　　　　　┃　｜　｜
					　19┠──────┨RS　　 Vss┠─●─●┐
					　　┃　　　　　　┗━━━━━┛　　　┯┿┯
					　　

	port:	port指定  IO名 ピン番号    共用 	 主な用途
			port= 0 	DIO0	XBee_pin 20 (Cms)		Commision コミッションボタン
			port= 1 	DIO1	XBee_pin 19 (AD1)		汎用入力用(DIN or AIN)
			port= 2 	DIO2	XBee_pin 18 (AD2)		汎用入力用(DIN or AIN)
			port= 3 	DIO3	XBee_pin 17 (AD3)		汎用入力用(DIN or AIN)
			port= 4 	DIO4	XBee_pin 11 			汎用出力用
			port= 5 	DIO5	XBee_pin 15 (Ass)		ASSOSIATE indication
			port= 6 	DIO6	XBee_pin 16 (RTS)		汎用出力用
			port=10 	DIO10	XBee_pin  6 (RSSI)		RSSI indication (PWM)
			port=11 	DIO11	XBee_pin  7 			汎用出力用
			port=12 	DIO12	XBee_pin  4 			汎用出力用

ソフト開発環境

	共通
			・X-CTU (Digi社)
			　PCもしくはH3694、Arduinoに接続する親機側のXBeeモジュールへ
			　Coordinatorのファームウェアを書き込むのに必要です。

				Device Type XBee Firmware	 Ver. ハードウェア
				-----------------------------------------------------
				Coordinator COORDINATOR API  2141 パソコンのシリアル端子
												  または秋月H8Tiny IO BOARD
												  またはArduino側に接続
				End Device	END DEVICE AT	 2841 単体で駆動

	Arduino用
			・Arduino IDE

	ARM mbed用
			・https://mbed.org/compiler/

	PC用
			・cygwin (http://www.cygwin.com/)
			・インストール時にdevl内のgcc coreとmakeを選択すること

	H3694用
			・GCC Developer Lite(株式会社ベストテクノロジー)
			　http://www.besttechnology.co.jp/modules/knowledge/

			上記ホームページの「ナレッジベース」より、技術情報／ソフト
			ウェア／GCC Developer Liteからダウンロードできます。
			この開発環境に含まれるH3694用のターゲットファイルを使用し
			ていますので、他の環境で開発する場合は注意が必要です。
*/

/*********************************************************************
インポート処理
*********************************************************************/

#ifndef LCD_TYPE_H
	#define LCD_TYPE_H
	#ifdef ARDUINO
		#if defined(__AVR_ATmega32U4__)
			#define LEONARDO	// Arduino LEONARDを使用する場合に自動的に定義されます。
		#endif
	//	#define LCD_H		// 本ライブラリ動作を液晶表示する場合に定義する
	//	#define ADAFRUIT	// 使用する液晶が Adafruit LCD Sheild の時に定義する
	#else // H8 or ARM or PC
		#ifndef ARM_MBED	// H8 or PC
			#define LCD_H
		#endif
	#endif
//	#define XBEE_WIFI	// XBee Wi-Fiを使用するときに定義する
//	#define XBEE_WIFI_DEBUG
//	#define LITE		// ライトモード(メモリ節約・機能制限)の時に定義する
	#define EASY_GPI	// xbee_gpi関数(リモートGPIO)を使用するときに定義する
	#define EASY_ADC	// xbee_adc関数(リモートADC)を使用するときに定義する
//	##define EASY_SENSOR
#endif

#ifndef XB_IMPORT_H
	#ifdef H3694
		#ifndef __3694_H__
			#include <3694.h>
		#endif
		#ifdef LCD_H
			#ifndef LCD_ROW_1
				#include "lcd_h8.c"
			#endif
		#endif
		/*	// lcd_h8ライブラリの関数
		void lcd_delay(unsigned int data);
		void lcd_toggle_E(void);
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
		*/
	#elif ARDUINO
		//	#include <inttypes.h>
		#include "Arduino.h"
		//	#include "WProgram.h"	// #if ARDUINO < 100
		#ifdef LCD_H
			#ifndef ADAFRUIT
				// 通常のキャラクタLCDの場合
				#include <LiquidCrystal.h>
			#else
				// Adafruit I2C接続LCDの場合(Adafruit_RGBLCDShieldライブラリが必要)
				#include <Wire.h>
				#include <Adafruit_MCP23017.h>
				#include <Adafruit_RGBLCDShield.h>
			#endif
		#endif
		#ifdef XBEE_WIFI
			#include <SPI.h>
			#include <Ethernet.h>
			#include <EthernetUdp.h>
		#endif
		#ifndef LCD_ROW_1
			#define 	LCD_ROW_1		0x00	//１行目先頭アドレス
			#define 	LCD_ROW_2		0x40	//２行目先頭アドレス
			#define 	LCD_ROW_3		0x14	//３行目先頭アドレス
			#define 	LCD_ROW_4		0x54	//４行目先頭アドレス
		#endif
	#else
		#ifdef ARM_MBED // ARM
		//	#include "mbed.h"
			#ifdef DEBUG
				#define LCD_H
				#define ERRLOG
				#define 	LCD_ROW_1		0x00	//１行目先頭アドレス
				#define 	LCD_ROW_2		0x40	//２行目先頭アドレス
				#define 	LCD_ROW_3		0x14	//３行目先頭アドレス
				#define 	LCD_ROW_4		0x54	//４行目先頭アドレス
			#endif
		#else // PC
			#include <stdio.h>
			#include <stdlib.h>
			#include <string.h>
			#include <termios.h>
			#include <sys/signal.h>
			#include <sys/time.h>
			#include <fcntl.h>
			#include <unistd.h>
			#include <arpa/inet.h>
			#ifdef XBEE_WIFI
				#include <sys/types.h>
				#include <sys/socket.h>
				#include <netinet/in.h>
				#include <ctype.h>
				#include <sys/ioctl.h>
				#include <string.h>
			#endif
			#define BAUDRATE B9600
			#ifndef LCD_ROW_1
				#include "lcd_pc.c"
			#endif
			#include <time.h>			// クロックタイマー用
		#endif
	#endif
	#define xbee_set_myaddress(adr) xbee_ratd_myaddress(adr)
#endif

/*********************************************************************
定数の設定
*********************************************************************/
#ifndef XB_DEFINE_H
	#define XB_DEFINE_H
	#ifndef NAME
		#ifdef LITE // BeeBee Lite by 蘭
			#define 	NAME		"BeeBee Lite"
			#define 	COPYRIGHT	"by Wataru & Ran"
		#else
			#define 	NAME		"ZB Coord"
			#define 	COPYRIGHT	"by Wataru KUNINO"
		#endif
	#endif

	#ifdef H3694
		#define ERRLOG
		#define LED1_OUT	IO.PDR8.BIT.B0	// LED赤の接続ポート(エラー用)
		#define LED2_OUT	IO.PDR8.BIT.B1	// LED緑の接続ポート(動作確認用)
		#define LCD_EN		IO.PDR8.BIT.B6	// 液晶用電源
		#define BUTN		IO.PDR8.BIT.B2	// ボタンの接続ポート
		#define SCI_SIZE	256 			// シリアルデータ長(大きすぎるとRAMが不足する)
		#define API_SIZE	128 			// 受信用APIデータ長(32～255)
		#define API_TXSIZE	64				// 送信用APIデータ長(32～255) シリアル送信最大長=API_TXSIZE-18バイト
		#define CALL_SIZE	32				// xbee_rx_call用戻りデータ(10～256)
		#define XB_AT_SIZE	32				// ATコマンドの最大長
	#elif ARDUINO
	//	#define LED1_OUT	15				// 赤色ＬＥＤ(エラー用)用デジタルポート(15=analog 1)
	//	#define LED2_OUT	16				// 緑色ＬＥＤ(動作確認用)用デジタルポート(16=analog 2)
	//	#define LCD_EN		 6				// 液晶用電源デジタルポート
	//	#define BUTN		14				// ボタンの接続ポート(14 = analog 0)
		#ifdef LITE // BeeBee Lite by 蘭
			#define 	API_SIZE	48		// 受信用APIデータ長(32～255)
			#define 	API_TXSIZE	34		// 送信用APIデータ長(32～255) シリアル送信最大長=API_TXSIZE-18バイト
			#define 	CALL_SIZE	16		// xbee_rx_call用戻りデータ(10～256)
			#define 	XB_AT_SIZE	16		// ATコマンドの最大長
		#else
			#define 	CACHE_RES	2		// 応答時のキャッシュ数（無効にするには定義を消す）
			#define 	API_SIZE	64		// 受信用APIデータ長(32～255)
			#define 	API_TXSIZE	64		// 送信用APIデータ長(32～255)
			#define 	CALL_SIZE	32		// xbee_rx_call用戻りデータ(10～256)
			#define 	XB_AT_SIZE	32		// ATコマンドの最大長
		#endif
		#define LIGHT			1
		#define TEMP			2
		#define HUMIDITY		3
		#define WATT			4
		#define BATT			5
		#define DISABLE 		0
		#define VENDER			1
		#define AIN 			2
		#define DIN 			3
		#define DOUT_L			4
		#define DOUT_H			5
	#else
		#ifdef ARM_MBED // ARM		＜条件は調整していない。ほぼArduinoのまま＞
			#define 	CACHE_RES	3		// 応答時のキャッシュ数（無効にするには定義を消す）
			#define 	API_SIZE	64		// 受信用APIデータ長(32～255)
			#define 	API_TXSIZE	64		// 送信用APIデータ長(32～255)
			#define 	CALL_SIZE	32		// xbee_rx_call用戻りデータ(10～256)
			#define 	XB_AT_SIZE	32		// ATコマンドの最大長
			#define 	delay(ms)		wait_millisec(ms)	// 関数名の複製
			#define LIGHT			1
			#define TEMP			2
			#define HUMIDITY		3
			#define WATT			4
			#define BATT			5
			#define DISABLE 		0
			#define VENDER			1
			#define AIN 			2
			#define DIN 			3
			#define DOUT_L			4
			#define DOUT_H			5
		#else // PC
			// #define DEBUG					// デバッグモード
			// #define DEBUG_TX 				// 送信パケットの表示
			// #define DEBUG_RX 				// 受信パケットの表示
			#define 	ERRLOG					// エラー時にログを出力
		//	#define 	XBEE_ERROR_TIME 		// エラー時のログに日時を付与
			#ifdef LITE // BeeBee Lite by 蘭
				#define 	API_SIZE	48		// 受信用APIデータ長(32～255)
				#define 	API_TXSIZE	34		// 送信用APIデータ長(32～255)
				#define 	CALL_SIZE	16		// xbee_rx_call用戻りデータ(10～256)
				#define 	XB_AT_SIZE	16		// ATコマンドの最大長
			#else
				#define 	CACHE_RES	5			// 応答時のキャッシュ数（無効にするには定義を消す）
				#define 	API_SIZE	128 		// 受信用APIデータ長(32～255)
				#define 	API_TXSIZE	64			// 送信用APIデータ長(32～255)
				#define 	CALL_SIZE	64			// xbee_rx_call用戻りデータ(10～256)
				#define 	XB_AT_SIZE	32			// ATコマンドの最大長
				#define 	delay(ms)		wait_millisec(ms)	// 関数名の複製
			#endif
		#endif
	#endif
	#define 	TIME_DEL	3				// デリミタ検出のタイムアウト時間(秒)
	#define 	MODE_AUTO	0x00			// 自動受信モード
	#define 	MODE_GPIN	0x92			// GPI data を受信するモード
	#define 	MODE_UART	0x90			// UART data を受信するモード
	#define 	MODE_UAR2	0x91			// UART data を受信するモード２(AO=1)
	#define 	MODE_SENS	0x94			// XB Sensorを受信するモード(1wire専用→通常のSensorはATISを使用)
	#define 	MODE_IDNT	0x95			// Node Identifyを受信するモード
	#define 	MODE_RES	0x88			// ローカルATコマンドの結果を受信
	#define 	MODE_RESP	0x97			// リモートATコマンドの結果を受信(仕様書はATNDで説明ATISもこれ)
	#define 	MODE_MODM	0x8A			// Modem Statusを受信
	#define 	MODE_TXST	0x8B			// UART Transmit Status を受信
	#define 	MODE_BATT	0xE1			// (独自定義)バッテリステータス RAT%Vの応答時
				// XBeeのFrame Typeに準拠する。
				// 注意：モードを増やしたときはxbee_from_acumとxbee_rx_call内の対応を追加すること
				// 独自定義の時はxbee_rx_call内のみ。
				// MODE値はenum定義にしない。

	#define 	STATUS_OK		0x00		// ATコマンドの結果がOK
	#define 	STATUS_ERR		0x01		// ATコマンドの結果がERROR
	#define 	STATUS_ERR_AT	0x02		// 指定されたATコマンドに誤りがある
	#define 	STATUS_ERR_PARM 0x03		// 指定されたパラメータに誤りがある
	#define 	STATUS_ERR_AIR	0x04		// リモートATコマンドの送信の失敗(相手が応答しない)

	#define 	MODM_RESET		0x01		// ローカルのXBeeがリセットした
	#define 	MODM_WATCHDOG	0x02		// ローカルのXBeeがWatch dogタイマーによってリセットした
	#define 	MODM_JOINED 	0x03		// (RouterまたはEnd Deviceで使用しているときに)ネットワークJoinした
	#define 	MODM_LEFT		0x04		// ネットワークからdis_assosiateした
	#define 	MODM_STARTED	0x06		// (coordinatorで使用しているときに)Coordinatorを開始した

	#define 	DEV_TYPE_XBEE	0x00		// XBeeモジュール
	#define 	DEV_TYPE_RS232	0x05		// RS-232Cアダプタ
	#define 	DEV_TYPE_SENS	0x07		// Sensor (1wire専用)
	#define 	DEV_TYPE_WALL	0x08		// Wall Router
	#define 	DEV_TYPE_SEN_LT 	0x0E		// Sensor (照度・温度)
	#define 	DEV_TYPE_SEN_LTH	0x0D		// Sensor (照度・温度・湿度)
	#define 	DEV_TYPE_PLUG	0x0F		// Smart Plug

	#define 	ZB_TYPE_COORD	0x21		// ZigBee Coordinator
	#define 	ZB_TYPE_ROUTER	0x23		// ZigBee Router
	#define 	ZB_TYPE_ENDDEV	0x29		// ZigBee End Device
	#define 	XB_TYPE_NULL	0x00		// XBee Wi-Fi バージョン未取得
	#define 	XB_TYPE_WIFI10	0x10		// XBee Wi-Fi Ver. 10xx
	#define 	XB_TYPE_WIFI20	0x20		// XBee Wi-Fi Ver. 20xx

	#define 	NET_ADR 	FFFE			// ネットワークアドレス
#endif

/*********************************************************************
型の定義とグローバル変数の宣言
*********************************************************************/
#ifndef XB_GLOBAL_H
	#define XB_GLOBAL_H
	typedef unsigned char byte; 			// Arduinoでも必要
	#ifdef H3694
		byte		TIMER_SEC	=	0	;	//ＲＴＣカウント用１秒単位
		char sci_tx[SCI_SIZE];				// シリアル用
		char sci_rx[SCI_SIZE];
	#elif ARDUINO
		#define TIMER_SEC	time1s256()
	#else
		#ifdef ARM_MBED
			#define TIMER_SEC	time1s256()
		#else  // PC
			#define TIMER_SEC	time1s256() 	// TIMER_SECのカウントアップの代わり
			volatile byte LED1_OUT;
			volatile byte LED2_OUT;
			volatile byte LCD_EN;
		#endif
	#endif

	#ifdef ARDUINO
		#ifdef LCD_H
			#ifndef ADAFRUIT
				// 通常のキャラクタLCDの場合
				LiquidCrystal xbee_ardlcd(8, 9, 4, 5, 6, 7);
								//LCD設定 rs en d0 d1 d2 d3
			#else
				// Adafruit I2C接続LCDの場合(Adafruit_RGBLCDShieldライブラリが必要)
				Adafruit_RGBLCDShield xbee_ardlcd = Adafruit_RGBLCDShield();
			#endif
		#endif

		// 構造体の宣言
		typedef struct{
			byte MODE;				// 受信モード(Frame Type)
			byte FROM[8];			// 送信元IEEEアドレス
			byte AT[2]; 			// ATコマンド
			byte ID;				// 応答パケットID(Frame ID)
			byte STATUS;			// 応答結果(0:OK 1:ERROR)／AT結果／UART状態
			union { 				// GPIOデータ
				byte BYTE[2];
				struct {			// バイト毎に下位ビットから代入(リトルエンディアン)
					byte D0 :1; byte D1 :1; byte D2 :1; byte D3 :1; // BYTE[1]
					byte D4 :1; byte D5 :1; byte D6 :1; byte D7 :1;
					byte	:1; byte	:1; byte D10:1; byte D11:1; // BYTE[0]
					byte D12:1; byte	:1; byte	:1; byte	:1;
				} PORT;
			} GPI;
			//	byte GPI[2];			// GPIOデータ
			unsigned int ADCIN[4];	// ADCデータ
			byte DATA[CALL_SIZE];	// 受信データ
		} XBEE_RESULT;				// 構造体の型名
	#else
		#ifdef ARM_MBED // ARM
			// 構造体の宣言
			typedef struct{
				byte MODE;				// 受信モード(Frame Type)
				byte FROM[8];			// 送信元IEEEアドレス
				byte AT[2]; 			// ATコマンド
				byte ID;				// 応答パケットID(Frame ID)
				byte STATUS;			// 応答結果(0:OK 1:ERROR)／AT結果／UART状態
				union { 				// GPIOデータ
					byte BYTE[2];
					struct {			// バイト毎に下位ビットから代入(リトルエンディアン)
						byte D0 :1; byte D1 :1; byte D2 :1; byte D3 :1; // BYTE[1]
						byte D4 :1; byte D5 :1; byte D6 :1; byte D7 :1;
						byte	:1; byte	:1; byte D10:1; byte D11:1; // BYTE[0]
						byte D12:1; byte	:1; byte	:1; byte	:1;
					} PORT;
				} GPI;
				//	byte GPI[2];			// GPIOデータ
				unsigned int ADCIN[4];	// ADCデータ
				byte DATA[CALL_SIZE];	// 受信データ
			} XBEE_RESULT;				// 構造体の型名
		#else // PC
			enum xbee_sensor_type{ LIGHT,TEMP,HUMIDITY,WATT,BATT,PRESS,VALUE,TIMES,NA };	// センサタイプの型
			enum xbee_port_type{ DISABLE=0, VENDER=1, AIN=2, DIN=3, DOUT_L=4, DOUT_H=5 };
																	// GPIOの設定の型
			typedef struct{
				byte MODE;				// 受信モード(Frame Type)
				byte FROM[8];			// 送信元IEEEアドレス
				byte SHORT[2];			// 送信元ショートアドレス
				byte AT[2]; 			// ATコマンド
				byte ID;				// 応答パケットID(Frame ID)
				byte STATUS;			// 応答結果(0:OK 1:ERROR)／AT結果／UART状態
				union { 				// GPIOデータ
					byte BYTE[2];
					struct {
						#ifdef H3694	// H8ではバイト毎に上位ビットから代入(ビッグエンディアン)
						byte D7 :1; byte D6 :1; byte D5 :1; byte D4 :1; // BYTE[1]
						byte D3 :1; byte D2 :1; byte D1 :1; byte D0 :1;
						byte	:1; byte	:1; byte	:1; byte D12:1; // BYTE[0]
						byte D11:1; byte D10:1; byte	:1; byte	:1;
						#else			// PCではバイト毎に下位ビットから代入(リトルエンディアン)
						byte D0 :1; byte D1 :1; byte D2 :1; byte D3 :1; // BYTE[1]
						byte D4 :1; byte D5 :1; byte D6 :1; byte D7 :1;
						byte	:1; byte	:1; byte D10:1; byte D11:1; // BYTE[0]
						byte D12:1; byte	:1; byte	:1; byte	:1;
						#endif
					} PORT;
				} GPI;
				unsigned int ADCIN[4];	// ADCデータ
				byte ZCL[6];			// [0]送信元EndPoint, [1]宛先EndPoint, [2-3]クラスタID, [4-5]プロファイルID
				byte DATA[CALL_SIZE];	// ペイロードデータ／ZCLヘッダ＋受信データ
			} XBEE_RESULT;
		#endif
	#endif
	#ifdef CACHE_RES
		byte CACHE_MEM[CACHE_RES][API_SIZE];
		byte CACHE_COUNTER = 0;
	#endif
#endif

#ifdef XBEE_WIFI
byte ADR_MY[] = {0xFF,0xFF,0xFF,0xFF};
	#ifdef ARDUINO
		extern byte mac[6];
		EthernetUDP UdpXBeeR;
		EthernetUDP UdpXBeeT;
		EthernetUDP UdpXBeeU;
	#endif
#endif

/*********************************************************************
スタティック変数の宣言
*********************************************************************/

/* エラーログ用 */
#ifdef ERRLOG
static byte 	TIMER_ERR	=	0	;		//エラー経過時間１秒単位
static char 	ERR_LOG[API_TXSIZE-18];
static byte 	ERR_CODE=0x00;
#endif

/* IEEEアドレス(最小限の通信対象をライブラリ側で保持する)／複数のデバイスへの対応はアプリ側で行う*/
static byte ADR_FROM[]= 	{0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF};	//差出人のIEEEアドレス(変更可)

// 以下にお手持ちのXBeeのアドレスを予め設定しておくと毎回の検索が不要です。
static byte ADR_DEST[]= 	{0x00,0x13,0xA2,0x00,0x00,0x00,0x00,0x00};	//宛先のIEEEアドレス(変更可)

/* ショートアドレス／本ライブラリでの宛先指定はIEEEのみを使う */
static byte SADR_DEST[]=	{0xFF,0xFE};			//ブロード(ショート)アドレス
static byte PACKET_ID = 0;							//送信パケット番号

/* XBeeのデバイスタイプ ATVRの上２ケタ */
static byte DEVICE_TYPE = ZB_TYPE_COORD;			// Coord=0x21 Router=23 ED=29 Wi-Fi=10

#ifndef H3694
#ifndef ARDUINO
#ifndef ARM_MBED // PC
	#ifndef XBEE_WIFI
		static byte xbee_com_port;
		static int xbeeComFd;
		static struct termios oldtio; /* current serial port settings (現在のシリアルポートの設定を待避)*/
	//	static byte *receive_buffer;
	#else // XBEE_WIFI
		 int xbeeTSFd;							// XBee Wi-Fi 送信ソケット用 ディスクリプタ
		 int xbeeRSFd;							// XBee Wi-Fi 受信ソケット用 ディスクリプタ
		 int xbeeUSFd;							// XBee Wi-Fi UARTソケット用 ディスクリプタ
		 struct sockaddr_in xbeeT_addr; 		// XBee Wi-Fi 送信アドレス構造体変数
		 struct sockaddr_in xbeeR_addr; 		// XBee Wi-Fi 受信アドレス構造体変数
		 struct sockaddr_in xbeeU_addr; 		// XBee Wi-Fi UARTアドレス構造体変数
	#endif
#endif
#endif
#endif

/*********************************************************************
ハードウェア
*********************************************************************/

/* H8タイニーマイコン H3694用のタイマー設定、GPIOポート設定 */
#ifdef H3694
	void port_init(void){

		/*ポート1		  76543210*/
		IO.PMR1.BYTE  = 0b00000000; 	// モード(P1,5) 	入出力=0  その他=1
		IO.PCR1 	  = 0b11110110; 	// 入出力設定		入力  =0  出力	=1	B3=リザーブ
		IO.PUCR1.BYTE = 0b00000000; 	// プルアップ(P1,5) しない=0  する	=1
		IO.PDR1.BYTE  = 0b00000000; 	// アクセス 		Ｌ出力=0  Ｈ出力=1

		/*ポート8		  76543210*/   // 秋月 H8 Tiny I/O BOARD TERA2 [K-00207]用
		IO.PCR8 	  = 0b11111011; 	// 入出力設定		入力  =0  出力	=1
		IO.PDR8.BYTE  = 0b00000000; 	// アクセス 		Ｌ出力=0  Ｈ出力=1
	}

	/* タイマー初期化 */
	void timer_sec_init(void){
		DI;
		TA.TMA.BYTE 	=0b00001000;  /* 時計用クロック  */
	/*					   |  |||-|__ 分周比 000～111(高速時) 000～011(低即時)
				 固定値 ___|--||			 000:0.5120 sec.  000 1000 msec.
							   |			 001:0.2560 sec.  001  500 msec.
		 高速=0, 低速=1 _______|			 100:0.0160 sec.  010  250 msec.
										   111:0.0005 sec.	011   31.25 msec.*/

		IRR1.BIT.IRRTA	=0; 		  /* タイマーＡ割込みフラグのリセット */
		IENR1.BIT.IENTA =1; 		  /* タイマーＡ割込みを利用可能にする */
		EI;
	}

	/* AKI H8 TINY IO BOARD TERA2 用/PC用の接続デバイス設定*/
	byte led_red( const byte in ){
		if		( in == 0 ) LED1_OUT=0;
		else if ( in == 1 ) LED1_OUT=1;
		return( LED1_OUT );
	}
	byte led_green( const byte in ){
		if		( in == 0 ) LED2_OUT=0;
		else if ( in == 1 ) LED2_OUT=1;
		return( LED2_OUT );
	}
	byte button( void ){
		return( BUTN );
	}
	void lcd_enable( const byte in ){
		LCD_EN = in;
	}
#endif

/* ARM mbed 用 のタイマー設定 */
#ifdef ARM_MBED 		// http://mbed.org/handbook/Timer
	Timer _xbee_time;	// http://mbed.org/teams/Aerodyne/code/Timer/file/1d3fd5616c0a/Timer.cpp
	void _xbee_time_init(){
		_xbee_time.start();
	}
#endif

/* ＲＴＣ使用1秒カウント */
byte time1s256(){
	#ifdef H3694
		return(0x00);
	#elif ARDUINO
		return( (byte)(millis()/1000) );
	#else
		#ifdef ARM_MBED
		//	return( (byte)(_xbee_time.read_ms()/1024) );
			if(_xbee_time.read()>=262144.0f)_xbee_time.reset();
			return( (byte)(_xbee_time.read()) );
		//	return(0);
		#else //PC
			// char c='\0';
			// if (kbhit()) c = getch();
			time_t sec;
			time( &sec );
			return( (byte)sec );
		#endif
	#endif
}

/*	ＲＴＣによる割り込み処理（1秒毎）  */
#ifdef H3694
	void int_timera(void){
		led_green( TIMER_SEC & 0x01 );	// LEDの正常点滅
		#ifdef ERRLOG
			if( led_red( 0xFF ) == 1 ) TIMER_ERR++; 	// エラー秒数(LED_赤でエラー検出)
		#endif
		TIMER_SEC++;				// タイマーのカウントアップ
		IRR1.BIT.IRRTA=0;			// タイマーA割込フラグのリセット
	}
#endif

/* シリアル待ち時間の時間カウント用、ＲＴＣ使用1秒間に256カウント */
byte timera(void){
	#ifdef H3694
		return( (byte)TA.TCA );
	#elif ARDUINO
		return( (byte)( (millis()/4)%256 ) );
	#else
		#ifdef ARM_MBED
			return( (byte)((_xbee_time.read_ms()/4)%256) );
		//	return( (byte)(_xbee_time.read()*256) );
		//	return(0);
		#else //PC
			return( (byte)(clock()/(CLOCKS_PER_SEC/256))  );
		#endif
	#endif
}

/* ミリ秒待ち（250ms以下の高精度用） 入力範囲＝4～250 ms */
#ifndef ARDUINO // BeeBee Lite by 蘭
void wait_millisec_250( byte ms ){
	#ifdef H3694
		byte counter;

		if( ms < 0x04 ) ms = 0x04;
		counter = timera() + (byte)( ms>>2 );
		if( counter == timera() ) counter++;
		while( counter != timera() );
	#elif ARDUINO
		delay( (unsigned long) ms );
	#else
		#ifdef ARM_MBED
			wait((float)ms/1000);
		#endif
	#endif
}
#endif

/* ミリ秒待ち（30秒までの広範囲版） 入力範囲＝4～30,000 ms */
void wait_millisec( const unsigned int ms ){
	#ifdef H3694
		byte loops;
		if( ms < 250 ){
			wait_millisec_250( (byte)ms );
		}else{
			loops = (byte)( ms / 250);
			wait_millisec_250( (byte)(ms-loops*250) );
			while( loops > 0x00 ){
				wait_millisec_250( 0xFA );
				loops--;
			}
		}
	#elif ARDUINO
		delay( (unsigned long) ms );
	#else
		#ifdef ARM_MBED
			wait((float)ms/1000.0f);
		#else // PC
			time_t target;

			target = (time_t)(clock()/(CLOCKS_PER_SEC/1000)) + (time_t)ms;
			if( target >= (time_t)ms ){
				while( (time_t)(clock()/(CLOCKS_PER_SEC/1000)) <= target );
			}else{
				while( (time_t)(clock()/(CLOCKS_PER_SEC/1000)) > (time_t)ms );		// クロックがリセットされるまで待つ
				while( (time_t)(clock()/(CLOCKS_PER_SEC/1000)) <= target );
			}
		#endif
	#endif
}

/* 経過時間測定＝4～1000 ms 以内 単位は1/256秒毎 */
byte past_time(const byte time_from){
	return( timera() - time_from );
}

/* XBeeのADCの有効ポート数を調べる ADC1～3のみ 入力=バイトデータ 応答0～3個 */
byte xbee_adc_count( byte d ){
	return( ((d>>1)&0x01)+((d>>2)&0x01)+((d>>3)&0x01) );
}

/* XBee用シリアル通信ドライバ */
#ifndef ARDUINO
#ifndef H3694
#ifndef ARM_MBED
#ifndef XBEE_WIFI
	int open_serial_port(char *modem_dev){
		/*
		PC-UNIX関連ドキュメント
		「シリアルポートプログラミング」
		http://www006.upp.so-net.ne.jp/kasuya/linux/serial_io_programing.html
		writer:粕谷友章 kasuya@pd5.so-net.ne.jp
		*/
		struct termios newtio;
		speed_t speed = B9600;
		xbeeComFd=open(modem_dev,O_RDWR);
		if(xbeeComFd < 0){
			perror(modem_dev);
			return -1;
		}else{
			tcgetattr(xbeeComFd,&oldtio);
			newtio.c_iflag = 0;
			newtio.c_oflag = 0;
			newtio.c_cflag = 0;
			newtio.c_lflag = 0;
			bzero(newtio.c_cc,sizeof(newtio.c_cc));
			cfsetispeed( &newtio, speed );
			cfsetospeed( &newtio, speed );
			newtio.c_cflag = BAUDRATE | CLOCAL | CREAD ;
			newtio.c_cflag &= ~CSIZE;
			newtio.c_cflag |= CS8;
			newtio.c_oflag = 0;
			newtio.c_lflag = 0;
			newtio.c_cc[VMIN] = 0;
			newtio.c_cc[VTIME] = 0;
			tcflush(xbeeComFd,TCIFLUSH);
			tcsetattr(xbeeComFd,TCSANOW,&newtio);
		}
		return 0;
	}
	void close_serial_port(void){
		tcsetattr(xbeeComFd,TCSANOW,&oldtio);
		close(xbeeComFd);
	}
#else	// XBEE_WIFI（arduino除く）
	int open_serial_port_tx(const byte *address){				// modem_dev＝IPアドレスのポインタ
		byte i;
		in_addr_t ip=0; 								// 送信アドレス

		for(i=0;i<4;i++){
			ip *= 256;
			ip += (in_addr_t)(byte)address[3-i];		// アドレス順序は反転する
		}
		/* 送信アドレス設定 */
		memset(&xbeeT_addr, 0, sizeof(xbeeT_addr)); 	// xbeeT_addrの初期化
		xbeeT_addr.sin_family = AF_INET;				// アドレスファミリー AF_INET
		xbeeT_addr.sin_port = htons( 0xBEE );					// 送信ポート番号
		xbeeT_addr.sin_addr.s_addr = ip;						// 送信IPアドレス
		/* ソケット生成 */
		xbeeTSFd = socket(AF_INET, SOCK_DGRAM, 0);	// 送信用ソケットの生成
		#ifdef DEBUG
			printf("IP(TX)=%s\n", inet_ntoa( xbeeT_addr.sin_addr ) );
		#endif
		return 0;
	}
	int open_serial_port_rx( void ){				// modem_dev＝IPアドレスのポインタ
		/* 受信アドレス設定 */
		memset(&xbeeR_addr, 0, sizeof(xbeeR_addr)); 	// xbeeR_addrの初期化
		memset(&xbeeU_addr, 0, sizeof(xbeeU_addr)); 	// xbeeU_addrの初期化
		xbeeR_addr.sin_family = AF_INET;				// アドレスファミリー AF_INET
		xbeeU_addr.sin_family = AF_INET;				// アドレスファミリー AF_INET
		xbeeR_addr.sin_port = htons( 0xBEE );					// 受信ポート番号(3054)
		xbeeU_addr.sin_port = htons( 0x2616 );					// 受信ポート番号(9750 board-voip)
		xbeeR_addr.sin_addr.s_addr = htonl(INADDR_ANY); 		// 受信IPアドレス(ANY)
		xbeeU_addr.sin_addr.s_addr = htonl(INADDR_ANY); 		// 受信IPアドレス(ANY)
		/* ソケット生成 */
		xbeeRSFd = socket(AF_INET, SOCK_DGRAM, 0);	// 受信用ソケットの生成
		xbeeUSFd = socket(AF_INET, SOCK_DGRAM, 0);	// 受信用ソケットの生成
		/* バインド */
		return(
			bind(xbeeRSFd, (struct sockaddr *)&xbeeR_addr, sizeof(xbeeR_addr)) +
			bind(xbeeUSFd, (struct sockaddr *)&xbeeU_addr, sizeof(xbeeU_addr))
		);
	}
	int open_serial_port(const byte *modem_dev){				// modem_dev＝IPアドレスのポインタ
		/*	UDPソケットを生成する。戻り値：0＝成功、-1＝エラー
		入力はcharだけど中ではbyteとして扱う。byteアドレス→char入力→byte使用なので変換は不要 */
		open_serial_port_tx( modem_dev );
		return( open_serial_port_rx( ) );
	}
	void close_serial_port_tx(void){
		close(xbeeTSFd);
		#ifdef DEBUG
			printf("DEBUG:close xbee tx sock\n");
		#endif
	}
	void close_serial_port(void){
		close(xbeeTSFd);
		close(xbeeRSFd);
		close(xbeeUSFd);
		#ifdef DEBUG
			printf("DEBUG:close xbee tx and rx sock\n");
		#endif
	}
#endif
#endif	// not ARM
#endif	// not H3694
#endif	// not ARDUINO

#ifdef ARDUINO
#ifdef XBEE_WIFI
int open_serial_port(void){
	Ethernet.begin(mac);
	delay(1000);
	UdpXBeeU.begin(0x2616); 		// UART受信用のUDP開始
	UdpXBeeR.begin(0xBEE);			// URAT リモートATコマンド受信用のUDPの開始
	UdpXBeeT.begin(0xBEE);			// URAT リモートATコマンド送信用のUDPの開始
	return( 0 );
}
#endif	// XBEE_WIFI
#endif	// ARDUINO

/* シリアルの初期化 */
/*
#ifdef ARM_MBED
	RawSerial _xbee_serial(SERIAL_TX, SERIAL_RX); // USART2
	// RawSerial _xbee_serial(D1, D0); //
	#ifdef DEBUG
		RawSerial _xbee_debug(PA_9, NC);		  // USART1
	//	RawSerial _xbee_debug(PA_9, PA_10);		  // USART1
	#endif
#endif
*/

/* シリアル受信用コールバック(ARM専用) */
	/* Serial.readableで確認してからSerial.getcする方法だと、連続した2バイト以上の
	　データ列を受信できなかった。STマイクロの問題なのかmbedの問題かは不明だが、
	　ハードウェアの受信バッファ内のデータをうまく読めないものと思う。
	　したがって、割込みで取り込んでソフト受信バッファを実装することにした。
	　バッファサイズはとりあえず128バイト */
#ifdef ARM_MBED
#define ARM_RX_BUFFER	64
volatile byte _xbee_serial_buf[ARM_RX_BUFFER];
volatile byte _xbee_serial_bp=0;
volatile byte _xbee_serial_rp=0;
void _xbee_serial_callback(){
	_xbee_serial_buf[_xbee_serial_bp] = _xbee_serial.getc();
	if(_xbee_serial_bp < (ARM_RX_BUFFER-1))_xbee_serial_bp++;
	else{
		#ifdef DEBUG
		_xbee_debug.printf("ERR:Buffer Full(%d)\r\n",_xbee_serial_bp);
		#endif
	}
}
#endif

/* シリアルの初期化 */
byte sci_init( byte port ){
	#ifdef H3694
		SCI3_INIT(br9600, sci_tx, SCI_SIZE, sci_tx, SCI_SIZE);
		EI; 								// 割り込み許可
		return(1);
	#elif ARDUINO
		#ifndef XBEE_WIFI
			#ifdef LEONARDO
				Serial1.begin(9600);
			#else
				Serial.begin(9600);
			#endif
			return(1);
		#else // Arduino XBee_WIFI
			byte i;
			#ifdef XBEE_WIFI_DEBUG
				Serial.begin(9600); 		// TEST用
				Serial.println("Hello!");
			#endif
			open_serial_port();
			return(1);
		#endif
	#else
		#ifdef ARM_MBED
			_xbee_serial.baud(9600);
			_xbee_serial.attach(&_xbee_serial_callback);
			#ifdef DEBUG
				_xbee_debug.baud(38400);
			#endif
			return(1);
		#else // PC  の時(ZigBeeシリアル or WiFi_LAN)
			#ifndef XBEE_WIFI	// ZigBeeシリアル
				/* tasasaki様よりポート11～64の拡張対応方法を教えていただいて追加した。*/
				char modem_dev[12] = "/dev/ttyS00";

				if( port <= 10){
					modem_dev[9] = (char)( port - 1 + (byte)'0' );
					modem_dev[10]= '\0';
				}else if( port <= 64 ){
					snprintf(&modem_dev[9], 3, "%d", port - 1);
				}else{
					fprintf(stderr,"ERR:sci_init port=%d\n",port);
					return(0);
				}
				if( open_serial_port( modem_dev ) ){
					wait_millisec( 100 );
					close_serial_port();	// open出来ていないが念のために閉じる
					wait_millisec( 100 );
					fprintf(stderr,"Failed serial COM%d (%s)\n",port,modem_dev);
					port = 0;
				}else{
					fprintf(stderr,"Serial port = COM%d (%s)\n",port,modem_dev);
					xbee_com_port = port;
				}
				return( port );
			#else	// XBEE_WIFI PC用
				byte i,j;
				for(i=0;i<3;i++){
					if( open_serial_port( ADR_DEST ) < 0 ){
						fprintf(stderr,"Failed UDP[%d/3]:",i+1);
						for(j=0;j<4;j++) fprintf(stderr,"%d.",(int)ADR_DEST[j]);
						fprintf(stderr,"\b\n");
						wait_millisec( 100 );
						close_serial_port();	// open出来ていないが念のために閉じる
						wait_millisec( 900 );
					}else break;
				}
				#ifdef DEBUG
					printf("DEBUG:SCI init for WiFi done\n");
				#endif
				return( 3-i );
			#endif // Wifi
		#endif // ARM/PC
	#endif
}

/* シリアル送受信バッファクリア */
void sci_clear(void){
	#ifdef H3694
		SCI3_IN_DATA_CLEAR();
		SCI3_OUT_DATA_CLEAR();
	#elif ARDUINO
		#ifndef XBEE_WIFI
			#ifdef LEONARDO
				Serial1.flush();
			#else
				Serial.flush();
			#endif
		#endif
	#else
		#ifdef ARM_MBED
			while( _xbee_serial.readable() ){
				_xbee_serial.getc();
				wait(0.002);
			}
			while( !_xbee_serial.writeable() ){
				_xbee_serial.send_break();
				wait(1);
			}
		#else //PC
			#ifndef XBEE_WIFI	// ZigBee
				tcflush(xbeeComFd,TCIOFLUSH);
			#else	// XBEE_WIFI
				close_serial_port();
				sci_init( 0 );
			#endif
		#endif
	#endif
}

/* シリアル受信(1バイト) */
byte sci_read(byte timeout){
	#ifdef H3694
		byte timer;
		byte ret=0;
		timer = timera() + (timeout)+1; 	// timeout[ms] = timer/256*1000
		while( timer != timera() && SCI3_IN_DATA_CHECK() < 1 );
		if( SCI3_IN_DATA_CHECK() ) ret=(byte)SCI3_IN_DATA();
		return( ret );
	#elif ARDUINO	// 蘭様による改良あり
		#ifndef XBEE_WIFI
			byte timer;
			timer = timera() + (timeout)+1; 	// timeout[ms] = timer/256*1000
		//	led_green(0);
			#ifdef LEONARDO
				while( timer != timera() && Serial1.available() <= 0 );
			//	led_green(1);
				if( Serial1.available() > 0 ) return( (byte)Serial1.read() );
			#else
				while( timer != timera() && Serial.available() <= 0 );
			//	led_green(1);
				if( Serial.available() > 0 ) return( (byte)Serial.read() );
			#endif
		#else
			return(0x00);	// フレームで受信するのでバイト毎受信は不要
		#endif
	#else
		#ifdef ARM_MBED
			// http://mbed.org/users/mbed_official/code/mbed-src/docs/e8b66477f5bf/classmbed_1_1RawSerial.html
			int value = 0,i=0;
			volatile byte timer;

			timer = timera() + (timeout)+1;

			// while( timer != timera() && i == 0 ) i=_xbee_serial.readable();
			while( timer != timera() && i == 0 ) i=_xbee_serial_bp;
			if( i > 0 ){
				// value = _xbee_serial.getc();
				value = _xbee_serial_buf[_xbee_serial_rp];
				if(_xbee_serial_rp < (ARM_RX_BUFFER-1))_xbee_serial_rp++;
				if(_xbee_serial_bp <= _xbee_serial_rp ){
					_xbee_serial_bp=0;
					_xbee_serial_rp=0;
				}
			//	#ifdef DEBUG_RX
			//		_xbee_debug.printf("%c(%02X) ",value,value);
			//	#endif
			}
			if(value<0 || value>255) value=0;
			return( (byte)value );
		#else // PC
			/* 受信の有無の判断にFDの待ち受け関数selectを使用する。
			参考文献
			http://linuxjm.sourceforge.jp/html/LDP_man-pages/man2/select.2.html
			*/
			#ifndef XBEE_WIFI
				byte c;
				struct timeval tv;
				fd_set readfds;

				FD_ZERO(&readfds);
				FD_SET( xbeeComFd , &readfds);
				tv.tv_sec = 0;
				#ifdef LITE	// BeeBee Lite by 蘭
					/*
					if( timeout > 50 ){
						tv.tv_usec = timeout*600;
					}else if( timeout > 10 ){
						tv.tv_usec = timeout*750;
					}else{
						tv.tv_usec = 0;
					}
					*/
					// 今回は見送ります(本関数を呼び出す前に調整しているので不要)
					tv.tv_usec = timeout*1000;
				#else
					tv.tv_usec = timeout*1000;
				#endif
				if( select( (xbeeComFd+1), &readfds, NULL, NULL ,&tv ) ){
					read(xbeeComFd,(char *)&c,1);
				}else{
					c = 0x00;
				}
				return( c );
			#else	// XBEE_WIFI
				return(0);	// フレームで受信するのでバイト毎受信は不要
			#endif
		#endif
	#endif
}

#ifdef XBEE_WIFI
/* シリアル受信(1フレーム) 始めの４バイトは送信元のアドレス */
byte sci_read_frame(byte *data){
	#ifndef H3694
	#ifndef ARDUINO // XBEE_WIFI PC受信
		byte i,ret;
		int len=0;
		struct sockaddr_in xbeeF_addr;					// FROMアドレス入力用
		struct timeval tv;								// タイムアウト用
		fd_set readfds;

		/* 受信の有無の判断にFDの待ち受け関数selectを使用する。*/
		FD_ZERO(&readfds);								// FD初期化
		FD_SET(xbeeRSFd, &readfds); 					// 待ちソケットの登録
		tv.tv_sec = (long)0;
		tv.tv_usec = (long)9000;						// 9ms
		/* データ受信 */
		if( select( (xbeeRSFd+1), &readfds, NULL, NULL, &tv) > 0 ){
			len = sizeof(xbeeF_addr);
			len = recvfrom(xbeeRSFd, &(data[4]), (API_SIZE-14), 0, (struct sockaddr *)&xbeeF_addr, &len );
			if( len > 0 ){
				len += 4;
				#ifdef DEBUG
					printf("IP(RX)=%s\n", inet_ntoa( xbeeF_addr.sin_addr ) );
				#endif
				for(i=0;i<4;i++){
					data[i]=(byte)(xbeeF_addr.sin_addr.s_addr % (long)256);
					xbeeF_addr.sin_addr.s_addr /= (long)256;
				}
			}
		}else{	// 受信データが無い時はUARTを受信する
			FD_ZERO(&readfds);								// FD初期化
			FD_SET(xbeeUSFd, &readfds); 					// 待ちソケットの登録
			tv.tv_usec = (long)1000;						// 1ms
			if( select( (xbeeUSFd+1), &readfds, NULL, NULL, &tv) > 0 ){
				len = sizeof(xbeeU_addr);
				len = recvfrom(xbeeUSFd, &(data[6]), (API_SIZE-16), 0, (struct sockaddr *)&xbeeU_addr, &len );
				if( len > 0 ){								// データはdata[6]以降に入る
					data[4] = 0x00; // UART受信を示す。
					data[5] = len;	// UART長を示す（data[]長では無い）。
					len += 6;
					#ifdef DEBUG
						printf("IP(UART)=%s\n", inet_ntoa( xbeeU_addr.sin_addr ) );
					#endif
					for(i=0;i<4;i++){
						data[i]=(byte)(xbeeU_addr.sin_addr.s_addr % (long)256);
						xbeeU_addr.sin_addr.s_addr /= (long)256;
					}
				}
			}
		}
		if( len < 0 ) ret = 0; else if( len>255 ) ret = 0xFF; else ret =(byte)len;
		#ifdef DEBUG_RX
			if( ret ){
				printf("Recieved Packet rx[%3d]\nip0,ip1,ip2,ip3,urt,len,", len);
				for(i=6;i<len;i++) printf("%3d,",i);
				printf("\n");
				for(i=0;i<len;i++) printf(" %02X,",data[i]);	// dataはbyte型
				printf("\n");
				for(i=0;i<len;i++) if( isgraph(data[i]) )	printf("'%c',",data[i]); else printf("%3d,",(int)data[i]);
				printf("\n");
			}
		#endif
		return( ret );
	#else	// XBEE_WIFI ARDUINO受信
		int packetSize;
		IPAddress remote;
		byte i;
		byte ret=0;

		/* UdpXBeeR受信 */
		packetSize = UdpXBeeR.parsePacket();
		if(packetSize){
			remote = UdpXBeeR.remoteIP();
			for(i=0;i<4;i++) data[i]=(byte)remote[i];
			if( packetSize > (API_SIZE-14) ) ret = API_SIZE-14; //API_SIZEはIPのため-4で良いと思ったが、ZBフォーマット移行の-9-1-4が上限)
			else ret = (byte)packetSize;
			UdpXBeeR.read(&(data[4]), ret);
			ret += 4;
		}else{
		/* UdpXBeeU受信 */
			packetSize = UdpXBeeU.parsePacket();
			if(packetSize){
				remote = UdpXBeeU.remoteIP();
				for(i=0;i<4;i++) data[i]=(byte)remote[i];
				if( packetSize > (API_SIZE-16) ) ret = API_SIZE-16; //API_SIZEはIPのため-6で良いと思ったが、ZBフォーマット移行の-9-1-6が上限)
				else ret = (byte)packetSize;		// Ver 1.80で抜けていた(バグ)
				data[4] = 0x00; // UART受信を示す。
				data[5] = ret;	// UART長を示す（data[]長では無い）。
				UdpXBeeU.read(&(data[6]), ret);
				ret += 6;
			}
		}
		#ifdef XBEE_WIFI_DEBUG
			Serial.print("RX udp size=");
			Serial.print(ret);
			Serial.print(", ");
			for(i=0;i<ret;i++){
				Serial.print(data[i],HEX);
				Serial.print(" ");
			}
			Serial.println("");
		#endif
		return( ret );
	#endif
	#endif
}
#endif

/* シリアル送信バッファが空になるのを待つ */
#ifndef LITE
byte sci_write_check(void){
	#ifdef H3694
		byte timer;
		timer = timera() + 0x7F;	// timeout = 500ms
		while( (timer != timera()) && ( SCI3_OUT_DATA_CHECK() < 1 ) );
		return( (byte)SCI3_OUT_DATA_CHECK() );
	#elif ARDUINO
		return( 1 );	// 関数があるかもしれないので、そのうち要調査＆確認
	#else
		#ifdef ARM_MBED
			if( !_xbee_serial.writeable() ){
				wait(0.1);
				while( !_xbee_serial.writeable() ) sci_clear();
			}
			return( 1 );
		#else // PC
			#ifndef XBEE_WIFI	// ZigBee
				tcdrain( xbeeComFd );
			#endif
			return( 1 );
		#endif
	#endif
}
#endif // LITE

/* シリアル送信 */
byte sci_write( char *data, byte len ){
	byte ret=1; // 戻り値 0でエラー

	#ifdef H3694
		SCI3_OUT_STRINGB( data , (short)len );	// 戻り値なし
	#elif ARDUINO
		byte i;
		#ifdef XBEE_WIFI
			UdpXBeeT.beginPacket(ADR_DEST, 0xBEE );
		#endif
		for(i=0;i<len;i++){
			#ifndef XBEE_WIFI
				#ifdef LEONARDO
					ret = Serial1.write( data[i] ); // 戻り値は書き込みバイト数
				#else
					ret = Serial.write( data[i] );	// 戻り値は書き込みバイト数
				#endif
			#else // XBEE_WIFI ARDUINO
				ret = UdpXBeeT.write( data[i] );	// 戻り値は書き込みバイト数
				/*
					Serial.print("TX udp size=");
					Serial.print(len,DEC);
					Serial.print(", ");
					for(i=0;i<len;i++){
						Serial.print(data[i],HEX);
						Serial.print(" ");
					}
					Serial.println("");
				*/
			#endif
			if( ret == 0 ){
				i = len;		// break;
			}
		}
		#ifdef XBEE_WIFI
			UdpXBeeT.endPacket();
		#endif
	#else
		#ifdef ARM_MBED
			byte i;
			sci_write_check();
			for(i=0;i<len;i++){
				if( _xbee_serial.putc((int)data[i]) < 0 ) ret=0;
			}
			if(ret) ret=len;
		#else // PC
			#ifndef XBEE_WIFI
				byte i;
				for(i=0;i<len;i++){
					if(write(xbeeComFd,&data[i],1) != 1){
						fprintf(stderr,"sci_write ERR:d[%02d]=0x%02x\n",i,(byte)data[i]);
						ret = 0;
					}
				}
				#ifdef DEBUG_TX
					printf("Transmitted Packet tx[%3d] to ", len);
					for(i=0;i<4;i++) printf("%02X",ADR_DEST[i]);
					printf(" ");
					for(i=4;i<8;i++) printf("%02X",ADR_DEST[i]);
					printf("\n");
					for(i=0;i<len;i++) printf("%3d,",i);
					printf("\n");
					for(i=0;i<len;i++) printf(" %02X,",(byte)data[i]);	// dataはchar型
					printf("\n");
					for(i=0;i<len;i++) if( (byte)data[i]>=0x20 ) printf("'%c',",data[i]); else printf("%3d,",(int)((byte)data[i]));
					printf("\n");
				#endif
			#else // XBEE_WIFI
				#ifdef DEBUG_TX
					int i;
				#endif
				int ret_i;	// 戻り値 0でエラー
				ret_i = sendto(xbeeTSFd, data, len, 0, (struct sockaddr *)&xbeeT_addr, sizeof(xbeeT_addr));
				if( ret_i<0 ) ret=0; else if( ret_i>255 ) ret = 0xFF; else ret = (byte)ret_i;
				#ifdef DEBUG_TX
					printf("Transmitted Packet tx[%3d] to ", len);
					for(i=0;i<4;i++) printf("%d.",ADR_DEST[i]);
					printf(":0xBEE\n");
					for(i=0;i<len;i++) printf("%3d,",i);
					printf("\n");
					for(i=0;i<len;i++) printf(" %02X,",(byte)data[i]);	// dataはchar型
					printf("\n");
					for(i=0;i<len;i++) if( isgraph((byte)data[i]) ) printf("'%c',",data[i]); else printf("%3d,",(int)((byte)data[i]));
					printf("\n");
				#endif
			#endif
		#endif // ARM/PC
	#endif
	#ifdef DEBUG
		#ifdef ARM_MBED
			_xbee_debug.printf("DEBUG:TX OUT DONE(%d)\r\n",ret);
		#else
			printf("DEBUG:TX OUT DONE(%d)\n",ret);
		#endif
	#endif
	return( ret );
}

/* string byte操作 */
void strcopy(char *s1, const char *s2){ 			// string.hのstrcpyの真似版
	while( *s2 != 0x00 ) *s1++ = *s2++;
	*s1 = 0x00;
}
void bytecpy(byte *s1, const byte *s2, byte size){	// strcpyのバイト長可変版
	byte i;
	for(i=0; i< size ;i++ ){
		s1[i] = s2[i];
	}
}
byte bytecmp(byte *s1, const byte *s2, byte size){	// strcmpの簡易版（大小比較なし）
/*バイトデータの比較
	入力：byte *s1 = 比較データ1
	入力：byte *s2 = 比較データ2
	入力：byte size = 比較データ長
	出力：戻り値 = 0:一致 1:不一致
*/
	byte i=0;										// 同一なら0を応答
	for(i=0; i< size ;i++ ){
		if( s1[i] != s2[i] ) return(1);
	}
	return(0);
}

/* LCD用関数 for ARDUINO */	// H8版とPC版は別ファイル　lcd_h8.c　lcd_pc.c
#ifdef ARDUINO
#ifdef LCD_H
	void lcd_cls(void){
		xbee_ardlcd.clear();
	}
	void lcd_home(void){
		xbee_ardlcd.home();
	}
	void lcd_control(byte disonoff, byte curonoff, byte curblink){
	}
	void lcd_goto(const byte mesto){
		byte row=0;

		switch( mesto ){
			case LCD_ROW_1: row=0; break;
			case LCD_ROW_2: row=1; break;
			case LCD_ROW_3: row=2; break;
			case LCD_ROW_4: row=3; break;
		}
		xbee_ardlcd.setCursor(0,(int)row);
	}
	void lcd_shift(const byte data){
		xbee_ardlcd.write(' ');
	}
	void lcd_putch(const char data){
		xbee_ardlcd.write( data );
	}
	void lcd_putstr(const char *data){
		while(*data != 0) {
			xbee_ardlcd.write(*data);
			data++;
		}
	}
	void lcd_disp_bin(const byte x){
		// xbee_ardlcd.print( x, BIN );
		byte i;
		for (i=128;i>0;i>>=1){
			if ((x&i)==0){
				lcd_putch('0');
			}else{
				lcd_putch('1');
			}
		}
	}
	void lcd_disp_hex(const byte i){
		// xbee_ardlcd.print( i, HEX );
		byte hi,lo;
		hi=i&0xF0;				 // High nibble
		hi=hi>>4;
		hi=hi+'0';
		if (hi>'9'){
			hi=hi+7;
		}
		lo=(i&0x0F)+'0';		 // Low nibble
		if (lo>'9'){
			lo=lo+7;
		}
		lcd_putch((char)hi);
		lcd_putch((char)lo);
	}
	void lcd_disp_1(const unsigned int x){
		if (x<10){
			xbee_ardlcd.write((char)(x+0x30));
		}else if (x<16){
			xbee_ardlcd.write((char)(x-10+'A'));
		}else{
			xbee_ardlcd.write('X');
		}
	}
	void lcd_disp_2(unsigned int x){
		unsigned int y;
		if (x<100){
			y=x/10;
			xbee_ardlcd.write((char)(y+0x30));
			x-=(y*10);
			xbee_ardlcd.write((char)(x+0x30));
		}else{
			xbee_ardlcd.print("XX");
		}
	}
	void lcd_disp_3(unsigned int x){
		unsigned int y;
		if (x<1000){
			y=x/100;
			xbee_ardlcd.write((char)(y+0x30));
			x-=(y*100);
			y=x/10;
			xbee_ardlcd.write((char)(y+0x30));
			x-=(y*10);
			xbee_ardlcd.write((char)(x+0x30));
		}else{
			xbee_ardlcd.print("XXX");
		}
	}
	void lcd_disp_5(unsigned int x){
		unsigned int y;
		if (x<=65535){
			y=x/10000;
			xbee_ardlcd.write((char)(y+0x30));
			x-=(y*10000);
			y=x/1000;
			xbee_ardlcd.write((char)(y+0x30));
			x-=(y*1000);
			y=x/100;
			xbee_ardlcd.write((char)(y+0x30));
			x-=(y*100);
			y=x/10;
			xbee_ardlcd.write((char)(y+0x30));
			x-=(y*10);
			xbee_ardlcd.write((char)(x+0x30));
		}else{
			xbee_ardlcd.print("XXXXX");
		}
	}
	void lcd_init(void){
		xbee_ardlcd.begin(20, 4);			// 液晶の桁数×行数の設定
		xbee_ardlcd.clear();				// 表示クリア
	}
#endif
#endif

/* LCD用関数 for ARM_MBED */	// 注意＝液晶では無くシリアルに出力する
#ifdef ARM_MBED					// D8(PA_9)からログをUART(38400baud)出力します
#ifdef DEBUG
	void lcd_cls(void){
		_xbee_debug.printf("----------------\r\n");
	}
	void lcd_home(void){
		_xbee_debug.printf("\r\n");
	}
	void lcd_control(byte disonoff, byte curonoff, byte curblink){
	}
	void lcd_goto(const byte mesto){
		lcd_home();
	}
	void lcd_shift(const byte data){
		_xbee_debug.putc(' ');
	}
	void lcd_putch(const char data){
		_xbee_debug.putc( data );
	}
	void lcd_putstr(const char *data){
		while(*data != 0) {
			_xbee_debug.putc(*data);
			data++;
		}
	}
	void lcd_disp_bin(const byte x){
		byte i;
		for (i=128;i>0;i>>=1){
			if ((x&i)==0){
				lcd_putch('0');
			}else{
				lcd_putch('1');
			}
		}
	}
	void lcd_disp_hex(const byte i){
		byte hi,lo;
		hi=i&0xF0;				 // High nibble
		hi=hi>>4;
		hi=hi+'0';
		if (hi>'9'){
			hi=hi+7;
		}
		lo=(i&0x0F)+'0';		 // Low nibble
		if (lo>'9'){
			lo=lo+7;
		}
		lcd_putch((char)hi);
		lcd_putch((char)lo);
	}
	void lcd_disp_1(const unsigned int x){
		if (x<10){
			_xbee_debug.putc((char)(x+0x30));
		}else if (x<16){
			_xbee_debug.putc((char)(x-10+'A'));
		}else{
			_xbee_debug.putc('X');
		}
	}
	void lcd_disp_2(unsigned int x){
		unsigned int y;
		if (x<100){
			y=x/10;
			_xbee_debug.putc((char)(y+0x30));
			x-=(y*10);
			_xbee_debug.putc((char)(x+0x30));
		}else{
			_xbee_debug.printf("XX");
		}
	}
	void lcd_disp_3(unsigned int x){
		unsigned int y;
		if (x<1000){
			y=x/100;
			_xbee_debug.putc((char)(y+0x30));
			x-=(y*100);
			y=x/10;
			_xbee_debug.putc((char)(y+0x30));
			x-=(y*10);
			_xbee_debug.putc((char)(x+0x30));
		}else{
			_xbee_debug.printf("XXX");
		}
	}
	void lcd_disp_5(unsigned int x){
		unsigned int y;
		if (x<=65535){
			y=x/10000;
			_xbee_debug.putc((char)(y+0x30));
			x-=(y*10000);
			y=x/1000;
			_xbee_debug.putc((char)(y+0x30));
			x-=(y*1000);
			y=x/100;
			_xbee_debug.putc((char)(y+0x30));
			x-=(y*100);
			y=x/10;
			_xbee_debug.putc((char)(y+0x30));
			x-=(y*10);
			_xbee_debug.putc((char)(x+0x30));
		}else{
			_xbee_debug.printf("XXXXX");
		}
	}
	void lcd_init(void){
		_xbee_debug.printf("-----\r\n");
	}
#endif
#endif

#ifdef LCD_H
void lcd_disp(const char *s){
			lcd_cls();
			lcd_goto(LCD_ROW_1);
			lcd_putstr( s );
		#ifndef H3694
		#ifndef ARDUINO
			#ifdef ARM_MBED
				_xbee_debug.printf("\r\n");
			#else // PC
				lcd_putch( '\n' );
			#endif
		#endif
		#endif
}
#endif

/*********************************************************************
XBeeドライバ関数
*********************************************************************/

/*
		   00 01 02 03 04 05 06 07 08....
	ATNJ = 7E 00 05 08 01 4E 4A FF 5F
		  |  | len |mo|FI| NJ=0xFF|CS|
		  |  |	   |de|  |		  |__|___ Check Sum
		  |  |	   |  |  |________|______ AT コマンド
		  |  |	   |  |__|_______________ Frame ID
		  |  |	   |__|__________________ AT=0x08 Remote=0x17
		  |  |_____|_____________________ Data Lengrh (ATコマンド+2文字)
		  |__|___________________________ 固定値

	char *at "AT"	ATコマンド			len=4バイト～最大255 (フレーム8バイト～)
			 "RAT"	リモートATコマンド	len=15バイト～最大255 (フレーム19バイト～)
			 "TX"	データ送信コマンド	 len=15バイト～最大255 (フレーム19バイト～)
										※API_SIZE=32の時valueは12バイトまで
*/

/* (ドライバ)ATコマンド送信 */
byte xbee_at_tx(const char *at, const byte *value, const byte value_len){
/*
	処理：リモートＡＴコマンドの送信を行うXBeeドライバ部
	入力：ATコマンドat[] ＝ "AT**"はローカルATで、"RAT***"がリモートＡＴ
	　　　　　　　　　　　　TXがデータ送信モード
	　　　データvalue[]　＝ 各ＡＴコマンドで引き渡すデータ値
	　　　value_len　　　＝ その長さ
	戻り値：送信したAPIサービス長。送信しなかった場合は0
*/
	char data_api[API_TXSIZE];
	byte i;
	byte len;				// APIサービス長
	#ifndef XBEE_WIFI
	byte check=0xFF;		// チェックサム
	byte data_position=5;	// 送信データdata_api[]の何処にvalue[]を入れるか
	#endif
	byte ret=0;

	if( PACKET_ID == 0xFF ){
		PACKET_ID=0x01;
	}else{
		PACKET_ID++;
	}
	len=0;
	#ifndef XBEE_WIFI
		switch( at[0] ){
			case 'A':
			case 'F':
				if( at[1]=='T'){
					data_api[3]=(char)0x08; 		// ATコマンドモード
					if( at[0] == 'A') data_api[4]=(char)PACKET_ID;		// フレームID (at="ATxx")
					else data_api[4]=0x00;					// IDなし（at="FTxx")
					data_api[5]=(char)at[2];			// ATコマンド上位
					data_api[6]=(char)at[3];			// ATコマンド下位
					data_position=7;
					len=4;						// サービスデータにMD+FI+AT(2)バイトが入る
				}
				break;
			case 'R':
			case 'S':
				if( at[1]=='A' && at[2]=='T'){
					data_api[3]=(char)0x17; 	// リモートATコマンドモード
					if( at[0] == 'R') data_api[4]=(char)PACKET_ID;		// フレームID (at="RATxx")
					else data_api[4]=0x00;					// IDなし（at="SATxx")
					for( i=5 ; i<=12 ; i++)  data_api[i]=(char)ADR_DEST[i-5];
					for( i=13 ; i<=14 ; i++) data_api[i]=(char)SADR_DEST[i-13];
					data_api[15]=(char)0x02;	// apply changes
					data_api[16]=(char)at[3];	// ATコマンド上位
					data_api[17]=(char)at[4];	// ATコマンド下位
					data_position=18;			// value[]を入れる場所
					len=15; 					// サービスデータにMD+FI+ADR(8)+SAD(2)+OPT+AT(2)が入る
				}
				break;
			case 'T':
				if( at[1]=='X'){
					data_api[3]=(char)0x10; 	// TXデータ送信モード
					data_api[4]=(char)0x00; 	// フレームIDを使用しない(no responce)
					for( i=5 ; i<=12 ; i++)  data_api[i]=(char)ADR_DEST[i-5];
					for( i=13 ; i<=14 ; i++) data_api[i]=(char)SADR_DEST[i-13];
					data_api[15]=(char)0x00;	// ZigBeeホップ数
					data_api[16]=(char)0x00;	// 暗号化＝しない
					data_position=17;
					len=14; 					// サービスデータにMD+FI+ADR(8)+SAD(2)+OPT(2)が入る
				}
				break;
			#ifndef ARDUINO
			#ifndef ARM_MBED
			case 'Z':
				// [0]送信元EndPoint, [1]宛先EndPoint, [2-3]クラスタID, [4-5]プロファイルID
				// [6]Radius=00 [7]Option=00 [8]Fram Ctrl [9]SeqNum [10]Command [11-12] Attribute
				if( at[1]=='C' && value_len>12){
					data_api[3]=(char)0x11; 			// ZCL送信モード
					data_api[4]=(char)PACKET_ID;		// フレームID
					for( i=5 ; i<=12 ; i++)  data_api[i]=(char)ADR_DEST[i-5];
					for( i=13 ; i<=14 ; i++) data_api[i]=(char)SADR_DEST[i-13];
					data_position=15;
					len=12;
				}
				break;
			#endif
			#endif
			default:
				break;
		}
		if( len ){
			data_api[0]=(char)0x7E; 			// デリミタ
			data_api[1]=(char)0x00; 			// パケット長の上位(送らない前程)
			for( i=3 ; i < data_position ; i++) check -= (byte)data_api[i];
			if( value_len > 0 ){
				for( i=0 ; i<value_len; i++){
					data_api[data_position + i] = (char)value[i];
					check -= (byte)data_api[data_position + i];
					len++;
				}
			}
			data_api[2]    =(char)len;
			data_api[len+3]=(char)check;
			#ifdef LITE	// BeeBee Lite by 蘭
				sci_write( data_api, (byte)(len+4) );
				ret=len+3;
			#else
				check = sci_write_check();		// 以降 checkはシリアルバッファ確認に使用する
				/*シリアルデータ送信 */
				if(  check > 0 ){
					if( sci_write( data_api, (byte)(len+4) ) == 0 ){
						/* シリアルリセット */
						#ifndef H3694
						#ifndef ARDUINO
						#ifndef ARM_MBED // PC
							wait_millisec( 100 );
							close_serial_port();			// シリアルを閉じる
							wait_millisec( 300 );
							printf("RESET serial\n");
							while( sci_init( xbee_com_port )==0){	// 再度オープン
								wait_millisec( 3000 );
								close_serial_port();		// シリアルを閉じる
								wait_millisec( 3000 );
								printf("RESET serial\n");
							}
							wait_millisec( 300 );
							sci_write( data_api, (byte)(len+4) );	// 再送信
						#endif
						#endif
						#endif
					}
					ret=len+3;
				}else{
					#ifdef H3694
						led_red( 1 );
					#endif
					#ifdef ERRLOG
						strcopy( ERR_LOG , "ERR:TX Buffer Over" );
						ERR_CODE = check;
					#endif
					ret=0;
				}
			#endif
		}
	#else // XBEE_WIFI
		switch( at[0] ){
			case 'A':
			case 'F':
				if( at[1]=='T' ){
					data_api[0]=(char)0x02; 			// ATコマンドモード
					data_api[1]=(char)0x00; 			// 固定
					if( at[0] == 'A' ) data_api[2]=(char)PACKET_ID; 	// フレームID
					data_api[3]=(char)0x02;
					data_api[4]=(char)at[2];			// ATコマンド上位
					data_api[5]=(char)at[3];			// ATコマンド下位
					len=6;
				}
				break;
			case 'R':
			case 'S':
				if( at[1]=='A' && at[2]=='T' ){
					data_api[0]=(char)0x02; 			// ATコマンドモード
					data_api[1]=(char)0x00; 			// 固定
					if( at[0] == 'R'){
						data_api[2]=(char)PACKET_ID;	// フレームID
						data_api[3]=(char)0x02;
					}else{
						data_api[2]=(char)0x00;
						data_api[3]=(char)0x00;
					}
					data_api[4]=(char)at[3];			// ATコマンド上位
					data_api[5]=(char)at[4];			// ATコマンド下位
					len=6;
				}
			case 'T':
				if( at[1]=='X'){
					data_api[0]=(char)0x00; 			// シリアルデータモード
					data_api[1]=(char)0x00; 			// ackが必要なときは01
					len=2;	// datasheetでは3だが誤り。実装は2
				}
				break;
			default:
				break;
		}
		if( len ){
			if( value_len > 0 ){
				for( i=0 ; i<value_len; i++){
					data_api[len] = (char)value[i];
					len++;
				}
			}
			if(DEVICE_TYPE == XB_TYPE_WIFI20){
				#ifdef DEBUG
					printf("DEBUG:TX_10(%3d):",len);
					for(i=0;i<len;i++) printf("%02X,",data_api[i]);
					printf("\n");
				#endif
				len+=6;
				if( len <= API_TXSIZE ){
					for(i=len-6; i>0; i--){
						data_api[i+5]=data_api[i-1];
					}
					data_api[0]=0x42;
					data_api[1]=0x42;
					data_api[2]=0x0;
					data_api[3]=0x0;
					data_api[4]=0x0;
					data_api[5]=0x0;
				}
				#ifdef DEBUG
					printf("DEBUG:TX_20(%3d):",len);
					for(i=0;i<len;i++) printf("%02X,",data_api[i]);
					printf("\n");
				#endif
			}
			if( sci_write_check() > 0 ){	// XBEE_WIFIなのでifdef LITEは不要
				#ifdef XBEE_WIFI_DEBUG
					Serial.print("TX udp size=");
					Serial.print(len);
					Serial.print(", ");
					for(i=0;i<len;i++){
						Serial.print(data_api[i],HEX);
						Serial.print(" ");
					}
					Serial.println("");
				#endif
				#ifdef DEBUG
					printf("TX udp size=%d\n",len);
				#endif
				if( sci_write( data_api, len ) == 0 ){
					#ifdef H3694
						led_red( 1 );
					#endif
					#ifdef ERRLOG
						strcopy( ERR_LOG , "ERR:TX Write Error" );
						ERR_CODE = 0x00;
					#endif
					#ifdef XBEE_WIFI_DEBUG
						Serial.println("ERR:TX Write Error" );
					#endif
					#ifdef DEBUG
						printf("ERR:TX Write Error\n");
					#endif
					ret=0;
				}else ret = len;
			}else{
				#ifdef H3694
					led_red( 1 );
				#endif
				#ifdef ERRLOG
					strcopy( ERR_LOG , "ERR:TX Buffer Over" );
					ERR_CODE = 0x00;
				#endif
				#ifdef DEBUG
					printf("ERR:TX Buffer Over\n");
				#endif
				ret=0;
			}
		}
	#endif
	return( ret );
}

/* (ドライバ)ATコマンド受信 */
byte xbee_at_rx(byte *data){
/*
	リモートＡＴコマンドの応答を受信するXBeeドライバ部
	処理：XBeeからの受信データをdata[]へ代入(応答)する
	入出力：APIデータdata[] 前３バイトは'7E'+len(2) data[0]が0x00の場合はタイムアウト
	リターン：APIサービス長、0はタイムアウト
*/
	byte i;
	byte len;			// APIサービス長－３ (APIフレームの前３バイトとチェックサム１バイトを除く)
	#ifndef XBEE_WIFI
	unsigned int leni;	// 実際のAPIサービス長と使用するAPIサービス長の差(データ破棄用)
	byte check = 0xFF;	// チェックサム
	#endif
	byte ret=0;

	/* 受信処理 */
	#ifndef XBEE_WIFI	// ZigBee
		data[0] = sci_read( 1 );				// 1ms待ち受けで受信
		if( data[0] == 0x7E ) { 				// 期待デリミタ0x7E時
			for( i=1;i<=2;i++ ){
				#ifdef LITE	// BeeBee Lite by 蘭
					data[i] = sci_read( 35 );	// 1割だけ緩和しました。sci_read 32⇒35(Wataru)
				#else
					data[i] = sci_read( 50 );
				#endif
			}
			if(data[1] == 0x00) len = data[2];
			else len = 0xFF - 4;		// API長が255バイトまでの制約(本来は64KB)
			if( len > (API_SIZE-4) ) len = API_SIZE-4;
			leni = (unsigned int)data[1] * 256 + (unsigned int)data[2] - (unsigned int)len;
				// 通常は0。lenが本来の容量よりも少ない場合に不足分が代入されれる
			for( i=0 ; i <= len ; i++){ // i = lenはチェックサムを入力する
				#ifdef LITE	// BeeBee Lite by 蘭
					data[i+3] = sci_read( 35 );	// 1割だけ緩和しました。sci_read 32⇒35(Wataru)
				#else
					data[i+3] = sci_read( 50 );
				#endif
				if( i != len) check -= data[i+3];	// チェックサムのカウント
			}
			while( leni > 0 ){
				data[len+3] = sci_read( 50 );	// データの空読み(lenは固定)
				if( leni != 1 ) {
					check -= data[len+3];		// leni=0の時はCheck sumなので減算しない
				}
				leni--;
			}
			if( check == data[len+3] ) ret = len +3;
			else ret = 0;
			#ifdef DEBUG_RX
				lcd_cls(); lcd_goto(LCD_ROW_1);
				lcd_putstr("->");
				for( i=0 ; i <= len+3 ; i++){
					lcd_disp_hex( data[i] );
					if( i == 8 ) lcd_goto(LCD_ROW_2);
					if( i == 18 ) lcd_goto(LCD_ROW_3);
					if( i == 28 ) lcd_goto(LCD_ROW_4);
					if( i == 38 ) lcd_goto(LCD_ROW_1);
				}
				lcd_goto(LCD_ROW_4);
				lcd_putstr("ID=");
				lcd_disp_hex( PACKET_ID );
				lcd_putstr(":");
				lcd_disp_hex( data[4] );
				if( check != data[len+3]){
					lcd_putstr(" ER=");
				}else{
					lcd_putstr(" OK=");
				}
				lcd_disp_hex( check );
				lcd_putstr(":");
				lcd_disp_hex( data[len+3] );
			#endif
		}
	#else // XBEE_WIFI
		ret = sci_read_frame( data );

		if( ret >= 7){			// もともと10バイト以上にしていたけどUARTは最低7バイトからなので変更
			#ifdef XBEE_WIFI
				if(DEVICE_TYPE == XB_TYPE_WIFI20 && data[4] != 0x00){ // UARTのときは処理不要
					#ifdef DEBUG
						printf("DEBUG:RX_10(%3d):",ret);
						for(i=0;i<ret;i++) printf("%02X,",data[i]);
						printf("\n");
					#endif
					if( ret > 10 ){
						for(i=10; i<ret; i++)data[i-6]=data[i];
						ret -=6;
					}
					#ifdef DEBUG
						printf("DEBUG:RX_20(%3d):",ret);
						for(i=0;i<ret;i++) printf("%02X,",data[i]);
						printf("\n");
					#endif
				}
			#endif
			switch( data[4] ){
				case 0x82:	// Remote AT Resp.	アドレス4バイト＋基本レスポンス6バイト＋オプション0バイト以上
					if(ret >= 10){
						for(i=ret;i>=10; i--){
							data[i+8] = data[i];	// 破壊 18以降全て ※メモリリークするのでUdpXBeeR受信で制限している
						}
						data[17] = data[9]; 	// AT Command status
						data[16] = data[8]; 	// AT Command LSB
						data[15] = data[7]; 	// AT Command MSB
						data[14] = 0xFE;
						data[13] = 0xFF;
						data[4]=data[6];		// Frame ID 破壊 4
						for(i=0;i<4;i++){		// IPアドレス
							data[5+i]=data[i];	// 破壊 5,6,7,8
							data[9+i]=0x00; 	// 破壊 9,10,11,12
						}
						data[3]=0x97;			// rat res
						data[0]=0x7E;			// 受信成功
						ret += 9;
						data[1]=0x00;			// length上位
						data[2]=ret-3;			// length下位
					}
					break;
				case 0x04:	// IO RX Sample アドレス4バイト＋基本レスポンス6バイト＋オプション0バイト以上
					if(ret >= 10){
						for(i=ret;i>=6; i--){	// datasheetの誤り Number SamplesのOffsetはdatesheetでは3だが実際は2
												// これにIPの4バイトが追加されるので6
							data[i+9] = data[i];	// 破壊 15以降全て ※メモリリークするのでUdpXBeeR受信で制限している
						}
						data[14] = 0x02;		// Packet was a broadcast
						data[13] = 0xFE;
						data[12] = 0xFF;
						for(i=0;i<4;i++){		// IPアドレス
							data[4+i]=data[i];	// 破壊 5,6,7,8
							data[8+i]=0x00; 	// 破壊 9,10,11,12
						}
						data[3]=0x92;			// io sample
						data[0]=0x7E;			// 受信成功
						ret += 10;
						data[1]=0x00;			// length上位
						data[2]=ret-3;			// length下位
					}
					break;
				case 0x00:	// UART
					len = data[5];				// 破壊されるのでUART長はバックアップ
					for(i=ret;i>=6; i--){
						data[i+9] = data[i];	// 破壊 15以降全て ※メモリリークするのでUdpXBeeU受信で制限している
					}
					data[14] = 0x02;			// Packet was a broadcast
					data[13] = 0xFE;
					data[12] = 0xFF;
					for(i=0;i<4;i++){			// IPアドレス
						data[4+i]=data[i];		// 破壊 5,6,7,8
						data[8+i]=0x00; 		// 破壊 9,10,11,12
					}
					data[1]=0x00;				// 243バイトまでしか考慮しない
					data[2]=len + 0x0C; 		// 243バイトまでしか考慮しない
					data[3]=0x90;				// UART
					data[0]=0x7E;				// 受信成功
					ret += 10;
					break;
				default:
					for(i=ret;i>=7; i--){
						data[i+8] = data[i];	// 破壊 18以降全て ※メモリリークするのでUdpXBeeR受信で制限している
					}
					data[4]=data[6];			// Frame ID 破壊 4
					for(i=0;i<4;i++){			// IPアドレス
						data[5+i]=data[i];		// 破壊 5,6,7,8
						data[9+i]=0x00; 		// 破壊 9,10,11,12
					}
					data[3]=data[4];
					ret=0;
			}
			#ifdef DEBUG
				printf("DEBUG:RX_ZB(%3d):",ret);
				for(i=0;i<ret;i++) printf("%02X,",data[i]);
				printf("\n");
			#endif
		}else ret=0;
		#ifdef XBEE_WIFI_DEBUG
			Serial.print("RX api size=");
			Serial.print(ret);
			Serial.print(", ");
			for(i=0;i<ret;i++){
				Serial.print(data[i],HEX);
				Serial.print(" ");
			}
			Serial.println("");
		#endif
	#endif
	return( ret );
}

/* (ドライバ)パケット差出人の抽出 */
byte xbee_from_acum( const byte *data ){
/*
	処理：受信データの差出人をグローバル変数へ登録 ※xbee_at_rxでは登録しない
	入力：data[] APIデータ
	出力：byte 受信データの種別mode値を戻り値に代入
	　　　受信データの差出人をグローバル変数ADR_FROMに代入
	再起動で8Aのモード応答あり
*/
	byte i;
	byte ret=0;

	if( data[3]==MODE_UART			// 0x90 UART Receive
	 || data[3]==MODE_UAR2			// 0x91 UART AO=0
	 || data[3]==MODE_GPIN			// 0x92 GPI data
	 || data[3]==MODE_SENS			// 0x94 XB Sensor
	 || data[3]==MODE_IDNT){		// 0x95 Node Identify
		for(i=0;i<8;i++) ADR_FROM[i]=data[4+i];
		ret = data[3];				// mode値
	}else if(
		data[3]==MODE_RESP){		// 0x97 リモートATコマンドの結果
		for(i=0;i<8;i++) ADR_FROM[i]=data[5+i];
		ret = data[3];				// mode値
	}else if(
		data[3]==MODE_RES			// 0x88 ローカルATコマンドの結果
	 || data[3]==MODE_MODM){		// 0x8A Modem Statusを受信
		ret = data[3];				// mode値
	}else{
		#ifdef H3694
			led_red( 1 );
		#endif
		#ifdef ERRLOG
			strcopy( ERR_LOG , "ERR:xbee_from" );
			ERR_CODE = data[3];
		#endif
		ret = 0;
	}
	return( ret );
}

/* (ドライバ)GPIO入力データの計算部 */
byte xbee_gpi_acum( byte *data ){
/*	リモート先(ADR_FROMに入力)のGPIOの入力値（下位バイト）を応答する。
	リモート先は（こちらから呼ばなくても）GPIOの変化などで自動送信する設定にしておく必要がある。
	具体的にはリモート先をATIC設定するなど。本演算関数はxbee_rx_callから呼ばれる。
	port:	port指定  IO名 ピン番号 		   USB評価ボード(XBIB-U-Dev)
			port= 1 	DIO1	XBee_pin 19 (AD1)		 SW2
			port= 2 	DIO2	XBee_pin 18 (AD2)		 SW3
			port= 3 	DIO3	XBee_pin 17 (AD3)		 SW4
	out:	port 1～7の入力値をバイトで応答。
			上位バイトのポートには対応しない
			DIOのport1～7がオフでかつADCが有効の場合は有効ADC(1～3の最も若い番号)のAD変換結果を応答する。
			ADC結果は8ビットに間引かれる。電圧は0～1.2Vに対応する。以下でADC電圧を得ることが可能。
					(unsigned int)xbee_gpi_acum(data)/255*1200 [mV]
	注意１：DIO/ADCの切り換え方法：DIOのport1～7が一つでも有効であればDIOとして動作します。
			port1～7のDIOが全て無効でADCが有効であれば有効なADC(1～3の最も若い番号)が動作します。
			port1～7のDIOとADC1～3が全て無効で電源電圧検出ADCが有効であれば電圧を応答します。(要確認)
			未対応：電源電圧ADCの範囲と単位を要確認。必要に応じて値を変換する必要がある。
	mode = MODE_GPIN(0x92)	 GPI data

	data[17]:mask(下位)
*/
	byte ret=0xFF;

	if( data[3] == MODE_GPIN ){								 		// data[3]:MODE_GPIN(92)
		if( xbee_from_acum( data ) > 0 ){							// 差出人をグローバル変数へ
			if( (data[17]&0xFE) != 0x00 ){							// DIO(port 1～7)がある時
				ret = (data[17] & data[20]);						// 20:DIOデータ(下位バイト) 17:マスク
			}else if( (data[18]&0x8E) != 0 ){						// ADCがあるとき
				if( data[16] == 0x00 && data[17] == 0x00 ){ 		// DIOが全てマスクされているとき
					ret = (data[19]<6) | (data[20]>2);				// 19:ADCデータ(上位)、20:下位
				}else{												//
					ret = (data[21]<6) | (data[22]>2);				// 21:ADCデータ(上位)、22:下位
				}
			}
		}else{
			#ifdef H3694
				led_red( 1 );
			#endif
			#ifdef ERRLOG
				strcopy( ERR_LOG , "ERR:xbee_gpi acum" );
				ERR_CODE = xbee_from_acum( data );
			#endif
		}
	}else{
		#ifdef H3694
			led_red( 1 );
		#endif
		#ifdef ERRLOG
			strcopy( ERR_LOG , "ERR:xbee_gpi mode" );
			ERR_CODE = data[3];
		#endif
	}
	#ifdef DEBUG_RX
		lcd_goto(LCD_ROW_2);
		lcd_putstr(" M:");				// MASK表示
		lcd_disp_hex( data[16] );
		lcd_disp_hex( data[17] );
		lcd_putstr(" D:");				// デジタルサンプル値
		lcd_disp_hex( data[19] );
		lcd_disp_hex( data[20] );
	#endif
	return(ret);
}

/* (ドライバ)UART入力データの計算部 */
byte xbee_uart_acum( byte *data ){
/*
	mode =MODE_UART(0x90)	UART Receive
	mode =MODE_UAR2(0x91)	UART AO=0
*/
	#ifdef DEBUG_RX
	byte i; // DEGUG用
	byte len;
	#endif
	byte ret=0;

	if( data[3] == 0x90 ){	// data[3]:データ種別=ZigBee Recieve Packet
		if( xbee_from_acum( data ) > 0 ){
			ret = data[15]; 					// 15:データ１文字 ※Digi仕様書P100記載誤り
			#ifdef DEBUG_RX
				len = data[2]-0x0C; 				// 12バイト減算
			#endif
		}else{
			#ifdef H3694
				led_red( 1 );
			#endif
			#ifdef ERRLOG
				strcopy( ERR_LOG , "ERR:xbee_uart from" );
				ERR_CODE = xbee_from_acum( data );
			#endif
		}
	}else if( data[3] == 0x91 ){	// data[3]:データ種別=ZigBee Explict Rx Indicator
		if( xbee_from_acum( data ) > 0 ){
			ret = data[21]; 					// 14:データ１文字
			#ifdef DEBUG_RX
				len = data[2]-0x12; 				// 18バイト減算
			#endif
		}else{
			#ifdef H3694
				led_red( 1 );
			#endif
			#ifdef ERRLOG
				strcopy( ERR_LOG , "ERR:xbee_uart2 from" );
				ERR_CODE = xbee_from_acum( data );
			#endif
		}
	}else{
		#ifdef H3694
			led_red( 1 );
		#endif
		#ifdef ERRLOG
			strcopy( ERR_LOG , "ERR:xbee_uart mode" );
			ERR_CODE = data[3];
		#endif
	}
	#ifdef DEBUG_RX
		lcd_goto(LCD_ROW_3);
		lcd_putstr("[");
		if( data[3]==0x90 && len>1 ) for(i=0; i< len ; i++) lcd_putch( data[15+i] );
		else if( data[3]==0x91 && len>1 ) for(i=0; i< len ; i++) lcd_putch( data[21+i] );
		else lcd_putch( ret );
		lcd_putstr( "](0x" );
		lcd_disp_hex( ret );
		lcd_putstr( ") l=" );
		lcd_disp_hex( len );
		if( data[3]==0x91 ){
			lcd_putstr(" CI:");
			lcd_disp_hex( data[16] );
			lcd_disp_hex( data[17] );
			lcd_putstr(".");
			lcd_disp_hex( data[18] );
			lcd_disp_hex( data[19] );
		}
	#endif
	return(ret);
}

/* (ドライバ)ATコマンドの送信＋受信 */
byte xbee_tx_rx(const char *at, byte *data, byte len){
/*
コマンド送信と受信がセットになったAPI(受信コマンドの簡単なチェックも実施)
	at: 	ATコマンド(文字列)
	data[API_SIZE]: 入力データ／出力データ(共用)
	※本コマンドの処理中に受信した他のパケット(送信IDとATコマンド名で判定)は破棄する。
	(始めに受信キャッシュクリアしている)
	戻り値：エラー時 0x00

注意：本コマンドは応答待ちを行うので干渉によるパケット損失があります。
*/

	byte err,retry;
	byte r_dat = 10;	// AT=0、RAT=10、TX=未定	リモートATと通常ATの応答値dataの代入位置の差
	byte r_at = 1;		// AT=0、RAT=1				 リモートの可否
	#ifndef XBEE_WIFI // ZigBee
	unsigned int wait_add = 0;
	#endif
	#ifdef CACHE_RES
		byte i;
	#endif

	#ifndef LITE	// BeeBee Lite by 蘭
		sci_write_check();
	#endif
	#ifdef H3694
		sci_clear();
	#endif

	#ifndef XBEE_WIFI // ZigBee
		if( at[0] == 'A' && at[1] == 'T' ){
			r_dat = 0; r_at=0;
			if( at[2] == 'W' && at[3] == 'R' ) wait_add = 100;			// ATWR 110～1100ms
		} else if ( at[0] == 'R' && at[1] == 'A' && at[2] == 'T' ){
			r_dat = 10; r_at=1;
			if( at[3] == 'W' && at[4] == 'R' ) wait_add = 100;	// RATWR 120～1200ms
		}
		if( xbee_at_tx( at ,data ,len ) > 0){
			err = 12;							// 受信なしエラー
			for( retry = 10 ; ( retry > 0 && err != 0 ) ; retry-- ){
				wait_millisec( 10 + (unsigned int)r_dat );	// 応答時間待ち AT 10～100ms / RAT 20～200ms
				if( wait_add != 0 ){
					wait_millisec( wait_add );	// 追加ウェイト
				}
				if( xbee_at_rx( data ) > 0){
					if( 	(
								( data[3] == MODE_RES  && r_at == 0 ) || ( data[3] == MODE_RESP && r_at == 1 )
							) && (
								data[4] == PACKET_ID
							)
						){
						 /* ATコマンドが正しいかどうかの確認を削除(Ver 1.51)
							※MODE_RESPの条件を追加したので不要のはず。
						 && data[5+r_dat] == (byte)at[2+r_at] && data[6+r_dat] == (byte)at[3+r_at]
						 */
						if( data[7+r_dat] == 0x00 ){
							err=0;
						}else err = data[7+r_dat];	// ATのERRORコード (AT:data[7] RAT:data[17])
					}else{				// 受信したパケットIDまたはATコマンドが相違しているとき
						#ifdef CACHE_RES
							if( CACHE_COUNTER < CACHE_RES ){
								for( i=0 ; i < API_SIZE ; i++) CACHE_MEM[CACHE_COUNTER][i] = data[i];
								CACHE_COUNTER++;
								#ifdef DEBUG
									lcd_putstr("### CACHE_RES ### <- ");	// DEBUG
									lcd_disp_hex( CACHE_COUNTER );
									lcd_putch('\n');
								#endif
							}
						#endif
						// キャッシュ容量が無い場合は受信データを保持せずにリトライする
					}
				}
			}
			//printf("\nretry=%d\n",retry);
		}else				err = 11;				// 送信失敗
	#else // XBEE_WIFI (PC + Arduino)
		#ifdef XBEE_WIFI_DEBUG
			for(i=0;i<(5+len);i++) Serial.print(at[i]);
			Serial.print('(');
			Serial.print(PACKET_ID,HEX);
			Serial.println(')');
		#endif

		if(DEVICE_TYPE == XB_TYPE_NULL){
			#ifdef DEBUG
				printf("DEBUG:check DEVICE_TYPE = XB_TYPE_WIFI20\n");
			#endif
			DEVICE_TYPE = XB_TYPE_WIFI20;
			if( xbee_tx_rx(at, data, len) > 0 ){
				return(1);
			}else{
				#ifdef DEBUG
					printf("DEBUG:check DEVICE_TYPE = XB_TYPE_WIFI10\n");
				#endif
				DEVICE_TYPE = XB_TYPE_WIFI10;
				if( xbee_tx_rx(at, data, len) > 0 ) return(1);
				else{
					#ifdef DEBUG
						printf("DEBUG:no Response at check DEVICE_TYPE\n");
					#endif
					DEVICE_TYPE = XB_TYPE_NULL;
					#ifdef ERRLOG
						ERR_CODE=12;
						strcopy( ERR_LOG ,"ERR:tx_rx no Rx Res. wifi check");
					#endif
					return(0); // エラー終了
				}
			}
		}
		if( xbee_at_tx( at ,data ,len ) > 0){
			err = 12;							// 受信なしエラー
			for( retry = 10 ; ( retry > 0 && err != 0 ) ; retry-- ){
				if( xbee_at_rx( data ) > 0){
					if( 	(
								( data[3] == MODE_RES  && r_at == 0 ) || ( data[3] == MODE_RESP && r_at == 1 )
							) && (
								data[4] == PACKET_ID
							)
						){
						 /* ATコマンドが正しいかどうかの確認を削除(Ver 1.51)
							※MODE_RESPの条件を追加したので不要のはず。
						 && data[5+r_dat] == (byte)at[2+r_at] && data[6+r_dat] == (byte)at[3+r_at]
						 */
						if( data[7+r_dat] == 0x00 ){
							err=0;
						}else err = data[7+r_dat];	// ATのERRORコード (AT:data[7] RAT:data[17])
					}else{				// 受信したパケットIDまたはATコマンドが相違しているとき
						#ifdef CACHE_RES
							if( CACHE_COUNTER < CACHE_RES ){
								for( i=0 ; i < API_SIZE ; i++) CACHE_MEM[CACHE_COUNTER][i] = data[i];
								CACHE_COUNTER++;
								#ifdef DEBUG
									lcd_putstr("### CACHE_RES ### <- ");	// DEBUG
									lcd_disp_hex( CACHE_COUNTER );
									lcd_putch('\n');
								#endif
							}
						#endif
						// キャッシュ容量が無い場合は受信データを保持せずにリトライする
					}
				}else{					// 受信データが無い時（リトライ前）
					wait_millisec(50);	// 50msの待ち時間
				}
			}
		}else err = 11; 			// 送信失敗
		delay(1);	// 直前のコマンド応答がすぐに返った時にキャッシュに貯めれないことを防止する

	#endif

	if( err ){
		wait_millisec( 1000 );		// 応答待ち状態で、次々にコマンドを送るとXBeeモジュールが非応答になる対策
		#ifdef H3694
			led_red( 1 );
		#endif
		#ifdef ERRLOG
			ERR_CODE=err;
			switch(err){
				case 2: 			// 01234567890123456789
					strcopy( ERR_LOG ,"ERR:tx_rx AT Command");
					break;
				case 3:
					strcopy( ERR_LOG ,"ERR:tx_rx AT Param. ");
					break;
				case 4:
					strcopy( ERR_LOG ,"ERR:tx_rx AT Commu. ");
					break;
				case 10:
					strcopy( ERR_LOG ,"ERR:tx_rx not AT CMD");
					break;
				case 11:
					strcopy( ERR_LOG ,"ERR:tx_rx TX Failed ");
					break;
				case 12:
					strcopy( ERR_LOG ,"ERR:tx_rx no Rx Res.");
					break;
				case 13:
					strcopy( ERR_LOG ,"ERR:tx_rx AT Pckt ID");
					break;
				case 14:
					strcopy( ERR_LOG ,"ERR:tx_rx Diff.Adrs.");
					break;
				default:
					strcopy( ERR_LOG ,"ERR:tx_rx AT unknown");
					break;
			}
			ERR_LOG[20] = ':';
			if( at[0] == 'A' ){
				ERR_LOG[21] = at[2];
				ERR_LOG[22] = at[3];
			}else{
				ERR_LOG[21] = at[3];
				ERR_LOG[22] = at[4];
			}
		#endif
	}
	#ifdef XBEE_WIFI_DEBUG
		if( err ){
			Serial.print("ERR in tx_rx ");
			Serial.println(err,DEC);
		}
	#endif
	#ifdef DEBUG
		#ifdef ARM_MBED
			if( err ){
				_xbee_debug.printf("DEBUG: %s ", ERR_LOG);
				_xbee_debug.printf("(err=%d)\r\n", err);
			}
		#else
			if( err ) fprintf(stderr,"DEBUG: %s (err=%d)\n", ERR_LOG,err);
		#endif
	#endif
	return( !err );
}

/***********************************************************************
XBee用 UART出力API
byte xbee_putch( const char c );
byte xbee_putstr( const char *s );
void xbee_disp_hex( const unsigned char i );
void xbee_disp_1( const unsigned int x );
void xbee_disp_2( unsigned int x );
void xbee_disp_3(unsigned int x);
void xbee_disp_5(unsigned int x);
void xbee_log( const byte level, const char *err , const byte x );
***********************************************************************/

#ifndef LITE	// BeeBee Lite by 蘭
byte xbee_putch( const char c ){
	byte data[2];
	byte len;
	data[0] = (byte)c;
	if( xbee_at_tx( "TX", data , 1) == 0 ) len=0; else len=1;
	return( len );
}
#endif

byte xbee_putstr( const char *s ){
/*
	文字を送信する
	入力：char *s
	出力：送信データ長を応答。０の場合は異常
*/
	byte data[API_TXSIZE-17];		// 17バイトはAPIヘッダ+CRC1バイトなのでデータ長は[API_TXSIZE-18]+null文字で+1する-17
	byte i;

	for(i=0; (i< (API_TXSIZE-18) ) && (s[i] != 0x00) ; i++){	// データ長はAPI_TXSIZE-18
		data[i] = (byte)s[i];		// テキストデータをバイナリデータ（バイト値）に変換する
	}
	data[i] = 0x00;
	if( xbee_at_tx( "TX", data , i) == 0) i=0;
	return( i );
}

#ifndef LITE	// BeeBee Lite by 蘭
void xbee_disp_hex( const byte i ){
	byte data[3];
	data[0] = i&0xF0;
	data[0] = data[0]>>4;
	data[0] += '0';
	if (data[0]>'9') data[0] += 7;
	data[1]=(i&0x0F)+'0';
	if (data[1]>'9') data[1]+=7;
	xbee_at_tx( "TX", data , 2);
}

void xbee_disp_1( const unsigned int x ){
	if		(x<10)	xbee_putch((char)(x+0x30));
	else if (x<16)	xbee_putch((char)(x-10+'A'));
	else			xbee_putch('X');
}

void xbee_disp_2( unsigned int x ){
	char s[3];
	unsigned int y;
	if (x<100){
		y=x/10; s[0]=(char)(y+0x30); x-=(y*10);
		s[1]=(char)(x+0x30);
		s[2]='\0';
		if( s[0]=='0' ){
			s[0]=' ';
		}
		xbee_putstr( s );
	}else xbee_putstr("XX");
}

void xbee_disp_3(unsigned int x){
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
		xbee_putstr( s );
	}else xbee_putstr("XXX");
}

void xbee_disp_5(unsigned int x){
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
		xbee_putstr( s );
	}else xbee_putstr("XXXXX");
}
#endif // LITE

void xbee_log(const byte level, const char *err, const byte x ){
/*
	エラーログ用レベルガイド
		LEVEL 5 ERROR	致命的なエラー、バグ発生
		LEVEL 4 ERR 	通信エラー等のうち不具合動作を伴う懸念のあるもの。500ms待ち挿入
		LEVEL 3 CAUTION 通信エラー、使い方の問題など
		LEVEL 2 未定義
		LEVEL 1 LOG 	各関数の動作確認用
*/
	#ifdef ERRLOG
		/* PCの場合 */
		#ifndef ARDUINO
		#ifndef H3694
		#ifdef XBEE_ERROR_TIME
			time_t error_time;
			struct tm *error_time_st;

			time(&error_time);
			error_time_st = localtime(&error_time);
		#endif
		#endif
		#endif

		#ifdef DEBUG
			if( level >= 1 ){
		#else
			if( level >= 3 ){
		#endif
			/* マイコンの場合 */
			#ifdef ARDUINO
				lcd_goto(LCD_ROW_4);
				lcd_putch('[');
				lcd_disp_hex( TIMER_SEC );
				lcd_putch(':');
				lcd_disp_hex( timera() );
				lcd_putch(']');
				lcd_putstr( err );
				lcd_putch('(');
				lcd_disp_hex( x );
				lcd_putch(')');
			#endif
			#ifdef H3694
				lcd_goto(LCD_ROW_4);
				lcd_putch('[');
				lcd_disp_hex( TIMER_SEC );
				lcd_putch(':');
				lcd_disp_hex( timera() );
				lcd_putch(']');
				lcd_putstr( err );
				lcd_putch('(');
				lcd_disp_hex( x );
				lcd_putch(')');
			#endif
			#ifdef ARM_MBED
				lcd_goto(LCD_ROW_4);
				lcd_putch('[');
				lcd_disp_hex( TIMER_SEC );
				lcd_putch(':');
				lcd_disp_hex( timera() );
				lcd_putch(']');
				lcd_putstr( err );
				lcd_putch('(');
				lcd_disp_hex( x );
				lcd_putch(')');
				lcd_putch( 0x0D );				// CR(\r)
				lcd_putch( 0x0A );				// LF(\n)
			#endif

			/*
			lcd_putch( 0x0D );				// CR(\r)
			lcd_putch( 0x0A );				// LF(\n)
			*/
			/* PCの場合 */
			#ifndef ARDUINO
			#ifndef H3694
			#ifndef ARM_MBED
				#ifdef XBEE_ERROR_TIME
					fprintf(stderr,"%4d/%02d/%02d %02d:%02d:%02d[%02X:%02X]%s(%02X)\n",
						error_time_st->tm_year+1900,
						error_time_st->tm_mon+1,
						error_time_st->tm_mday,
						error_time_st->tm_hour,
						error_time_st->tm_min,
						error_time_st->tm_sec,
						TIMER_SEC,
						timera(),
						err,
						x);
				#else
					fprintf(stderr,"[%02X:%02X]%s(%02X)\n",
						TIMER_SEC,
						timera(),
						err,
						x);
				#endif
			#endif
			#endif
			#endif
		}
		if( level > 3 ) wait_millisec( 500 );	// LEVEL 4以上に対して待ち時間を設定
	#endif
}

/***********************************************************************
アプリ向け提供関数
XBee用 各種制御 入出力定義を変更した場合は履歴に記載します
***********************************************************************/

byte xbee_reset( void ){
/*	XBeeモジュールのリセット
	戻り値：0＝エラー、強制終了あり(PC版)、無限定しあり(H8版)
*/
	#ifndef LITE	// BeeBee Lite by 蘭
	byte i;
	#endif
	byte ret=0;	// 戻り値 0:異常終了
	#ifndef XBEE_WIFI // ZigBee
		byte value[API_SIZE];
		value[0] = 0x00;
	#endif

	#ifndef XBEE_WIFI
	#ifndef LITE	// BeeBee Lite by 蘭
		sci_write_check();
	#endif
	sci_clear();						// シリアル異常をクリア
	DEVICE_TYPE = 0x20; 				// タイプ名を初期化
	#endif

	#ifdef LITE	// BeeBee Lite by 蘭
		ret = xbee_tx_rx( "ATVR", value ,0 );	// ZigBee 種類の取得
		if( ret == 0 ){
			#ifdef ARDUINO
				return(0);
			#else
				exit(-1);
			#endif
		}else{
			DEVICE_TYPE = value[8];
		}										// LITEはリセットしない
	#else // normal
		wait_millisec(100);
		for( i=1 ; i< 4 ; i++){
			#ifndef XBEE_WIFI // ZigBee
				ret = xbee_tx_rx( "ATVR", value ,0 );
			#else
				ret = 1;
			#endif
			if( ret > 0){
				#ifndef XBEE_WIFI
					DEVICE_TYPE = value[8];
					if( DEVICE_TYPE != ZB_TYPE_COORD &&
						DEVICE_TYPE != ZB_TYPE_ROUTER &&
						DEVICE_TYPE != ZB_TYPE_ENDDEV){ // VRの確認
						#ifdef LCD_H
							lcd_cls();
							#ifdef H3694
								led_red( 1 );
							#endif
							lcd_putstr( "EXIT:XBEE NOT IN API MODE" );
						#endif
						#ifdef H3694
							return(0);
						#elif ARDUINO
							return(0);
						#else
							exit(-1);
						#endif
					}
				#else // XBEE_WIFI
					DEVICE_TYPE = XB_TYPE_NULL;
				#endif
			}else{
				if( i == 3 ){
					#ifdef LCD_H
						lcd_cls();
						#ifdef H3694
							led_red( 1 );
						#endif
						lcd_putstr( "EXIT:NO RESPONCE FROM XBEE" );
					#endif
					#ifdef H3694
						return(0);
					#elif ARDUINO
						return(0);
					#else
						exit(-1);
					#endif
				}
				wait_millisec(1000);
			}
		}
		#ifndef XBEE_WIFI // ZigBee
			wait_millisec(100);
			ret = xbee_tx_rx( "ATFR", value ,0 );
			if( ret == 0){
				#ifdef LCD_H
					lcd_cls();
					#ifdef H3694
						led_red( 1 );
					#endif
					lcd_putstr( "EXIT:CANNOT RESET XBEE" );
				#endif
				#ifdef H3694
					return(0);
				#elif ARDUINO
					return(0);
				#else
					exit(-1);
				#endif
			}
			wait_millisec(3000);				// リセット指示後3秒後に起動
			sci_clear();						// 再起動のメッセージをクリア
		//	while( xbee_at_rx( value ) == 0 );	// パケットの破棄（永久ループの懸念がある）
			value[0] = 0x01;					// API MODE=1に設定
			xbee_tx_rx("ATAP", value , 1 );
			value[0] = 0x05;					// RSSI LEDを点灯
			xbee_tx_rx("ATP0", value , 1 );
			wait_millisec(500);
			value[0] = 0x01;					// RSSI LEDを受信強度に戻す
			xbee_tx_rx("ATP0", value , 1 );
		#endif
	#endif // LITE
	return( ret );
}

byte xbee_myaddress( byte *address ){
/*
自分自身のIEEEアドレスを取得する ／ XBee Wifi(PC)の場合は設定する
	byte *address : IEEEアドレスを代入する
	戻り値＝１で正常読み込み、２は書き込み。０は異常
*/
	#ifndef XBEE_WIFI
		byte data[API_SIZE];
		byte i=0;
		byte ret=0;

		data[0]=0x00;
		/* ショートアドレスの取得はしないことにする。(Coordinatorは常に00)
		if( xbee_tx_rx( "ATMY",data,0) ){
			for(i=0;i<2;i++){
				SADR_MY[i]=data[8+i];
			}
		}
		*/
		data[0]=0x00;
		if( xbee_tx_rx( "ATSH",data,0) ){
			for(i=0;i<4;i++){
				address[i]=data[8+i];
			}
			data[0]=0x00;
			if( xbee_tx_rx( "ATSL",data,0) ){
				for(i=0;i<4;i++){
					address[4+i]=data[8+i];
				}
				ret=1;
			}
		}
		#ifdef LCD_H
			if( ret==0 ) xbee_log( 5, "ERROR: at at_my" , 0 );
			xbee_log( 1, "done:xbee_myaddress" , ret );
		#endif
		return( ret );
	#else // XBEE_WIFI		/******* ADR_MYに登録されているIPアドレスを設定する **********/
		#ifdef ARDUINO
			byte i=0;
			IPAddress ip = Ethernet.localIP();
			for(i=0;i<4;i++) ADR_MY[i] = ip[i];
			for(i=0;i<4;i++) address[i] = ADR_MY[i];
			return( 1 );
		#else // PC
			byte i;
			byte ret=2;
			for(i=0;i<4;i++) if( ADR_MY[i] != 0xFF ) ret=1;
			if(ret==1) for(i=0;i<4;i++) address[i]=ADR_MY[i];	// ADR_MYが設定されているときは読み込む
			else for(i=0;i<4;i++) ADR_MY[i]=address[i]; // ADR_MYが全てFFの時は入力addressを設定する。
			#ifdef DEBUG
				printf("ADR_MY=");
				for(i=0;i<4;i++) printf("%d.",(int)ADR_MY[i]);
				printf("\b\n");
			#endif
			return( ret );
		#endif
	#endif
}

void xbee_address(const byte *address){
/*
送信用の宛先アドレス設定用の関数
	入力：byte *address = 宛先(子機)アドレス
*/
	byte i;

	#ifndef XBEE_WIFI // XBee ZB用
		for(i=0; i< 8 ;i++ ) ADR_DEST[i] = address[i];
		#ifndef ARDUINO
		#ifndef ARM_MBED
			SADR_DEST[0] = 0xFF;
			SADR_DEST[1] = 0xFE;
		#endif
		#endif
	#else // XBEE_WIFI
		#ifdef ARDUINO
			for( i=0;i<4;i++) ADR_DEST[i] = address[i];
		#else
			if( bytecmp(ADR_DEST,address,4) != 0 ){
				close_serial_port_tx();
				open_serial_port_tx( address );
				for( i=0;i<4;i++) ADR_DEST[i] = address[i];
			}else{
				#ifdef LCD_H
					xbee_log( 2, "same address" , address[7] );
				#endif
			}
		#endif
	#endif

	#ifdef DEBUG
		#ifndef XBEE_WIFI
			lcd_cls(); lcd_goto(LCD_ROW_1); lcd_putstr("DEST=");
			for(i=4;i<8;i++){ lcd_disp_hex( ADR_DEST[i] ); lcd_putstr(" "); }
		#else // XBEE_WIFI
			printf("DEST=");
			for(i=0;i<4;i++) printf("%d.",(int)ADR_DEST[i]);
			printf("\b\n");
		#endif
	#endif
	#ifdef LCD_H
		xbee_log( 1, "done:xbee_address" , address[7] );
	#endif
}

#ifndef XBEE_WIFI
#ifndef ARDUINO
#ifndef ARM_MBED
void xbee_short_address(const byte *address){
	SADR_DEST[0] = address[0];
	SADR_DEST[1] = address[1];
}
#endif
#endif
#endif

byte xbee_atd( const byte *address ){
/*
送信用の宛先アドレス設定用の関数 XBeeへの設定
	入力：byte *address = 宛先(子機)アドレス
	戻り値：戻り値＝１で正常、０は異常
*/
	#ifndef XBEE_WIFI
		byte data[API_SIZE];
		byte i;
		byte ret=0;

		for(i=0;i<4;i++)data[i]=address[i];
		if( xbee_tx_rx( "ATDH",data,4) ){
			for(i=0;i<4;i++)data[i]=address[i+4];
			if( xbee_tx_rx( "ATDL",data,4) ){
				ret=1;
			}
		}
		return( ret );
	#else // XBEE_WIFI
		return( 0 );
	#endif
}

byte xbee_ratd(const byte *dev_address, const byte *set_address ){
/*
指定したアドレスのXBee子機に任意のアドレスを（宛先として）設定する
	byte *dev_address : 設定先のXBeeデバイスのアドレス
	byte *set_address : 設定するアドレス
	戻り値＝XBeeデバイス名。0xFFは異常
*/
	byte i=0;
	#ifdef XBEE_WIFI
	byte len=0;
	#endif
	byte data[API_SIZE];
	byte dd=0xFF;			// デバイス名

	xbee_address(dev_address);								// 宛先のアドレスを設定
	#ifndef XBEE_WIFI
		for( i=0;i<4;i++) data[i]=set_address[i];				// 上位４バイトをdataに代入
		if( xbee_tx_rx( "RATDH", data ,4 ) > 0 ){				// 上位４バイトを設定
			for( i=0;i<4;i++) data[i]=set_address[i+4]; 		// 下位４バイトをdataに代入
			if( xbee_tx_rx( "RATDL", data ,4 ) > 0 ){			// 下位４バイトを設定
				if( xbee_tx_rx( "RATDD", data ,0 ) > 0 ){		// デバイス名を取得
					if( data[18]==0x00 && data[19]==0x03 && data[20]==0x00 ){	// XBeeデバイス
						dd=data[21];							// デバイス名をddに代入
					}
				}else{
					#ifdef LCD_H
						xbee_log( 4, "ERR:tx_rx RATDD at ratd" , 1 );
					#endif
				}
			}else{
				#ifdef LCD_H
					xbee_log( 4, "ERR:tx_rx RATDL at ratd" , 1 );
				#endif
			}
		}else{
			#ifdef LCD_H
				xbee_log( 4, "ERR:tx_rx RATDH at ratd" , 1 );
			#endif
		}
	#else // XBEE_WIFI
		for( i=0;i<4;i++){
			if(set_address[i] >= 100){
				data[len] = (byte)(set_address[i]/100);
				data[len+1] = (byte)(set_address[i]/10)-10*data[len]+(byte)'0';
				data[len] += (byte)'0';
				len += 2;
			}else if(set_address[i] >= 10){
				data[len] = (byte)(set_address[i]/10)+(byte)'0';
				len += 1;
			}
			data[len] = (byte)(set_address[i]%10)+(byte)'0';
			data[len+1] = (byte)'.';		// 最後のデータdata[len]に「.」が入るが
			len += 2;
		}
		len--;								// 最後の「.」は送信しない。
		if( xbee_tx_rx( "RATDL", data ,len ) > 0 ){ 		// 下位４バイトを設定
			dd = 0x00;
		}else{
			#ifdef LCD_H
				xbee_log( 4, "ERR:tx_rx RATDL at ratd" , 1 );
			#endif
		}
	#endif
	return(dd);
}

byte xbee_ratd_myaddress(const byte *address){
/*
指定したアドレスのXBee子機に本機のアドレスを（宛先として）設定する
	byte *address : 設定先のXBeeデバイスのアドレス
	戻り値＝XBeeデバイス名。0xFFは異常

*/
	byte i=0;
	#ifdef XBEE_WIFI
	byte len=0;
	#endif
	byte data[API_SIZE];
	byte adr_my[8];
	byte dd=0xFF;			// デバイス名

	if( xbee_myaddress(adr_my) ){								// 自分のアドレスを取得
		#ifdef DEBUG
			#ifndef XBEE_WIFI
				#ifdef ARM_MBED
					_xbee_debug.printf("adr_my =");
					for( i=0 ; i<8 ; i++) _xbee_debug.printf("%02x ",adr_my[i]);
					_xbee_debug.printf("\b\n");
				#else // PC
					printf("adr_my =");
					for( i=0 ; i<8 ; i++) printf("%02x ",adr_my[i]);
					printf("\b\n");
				#endif
			#else // XBEE_WIFI
				printf("adr_my =");
				for( i=0 ; i<4 ; i++) printf("%d.",adr_my[i]);
				printf("\b\n");
			#endif
		#endif
		xbee_address(address);								// 宛先のアドレスを設定
		#ifndef XBEE_WIFI
			for( i=0;i<4;i++) data[i]=adr_my[i];				// 上位４バイトをdataに代入
			if( xbee_tx_rx( "RATDH", data ,4 ) > 0 ){				// 上位４バイトを設定
				for( i=0;i<4;i++) data[i]=adr_my[i+4];			// 下位４バイトをdataに代入
				if( xbee_tx_rx( "RATDL", data ,4 ) > 0 ){			// 下位４バイトを設定
					if( xbee_tx_rx( "RATDD", data ,0 ) > 0 ){		// デバイス名を取得
						if( data[18]==0x00 && data[19]==0x03 && data[20]==0x00 ){	// XBeeデバイス
							dd=data[21];							// デバイス名をddに代入
						}
					}else{
						#ifdef LCD_H
							xbee_log( 4, "ERR:tx_rx RATDD at set_myadd" , 1 );
						#endif
					}
				}else{
					#ifdef LCD_H
						xbee_log( 4, "ERR:tx_rx RATDL at set_myadd" , 1 );
					#endif
				}
			}else{
				#ifdef LCD_H
					xbee_log( 4, "ERR:tx_rx RATDH at set_myadd" , 1 );
				#endif
			}
		#else // XBEE_WIFI
			for( i=0;i<4;i++){
				if(adr_my[i] >= 100){
					data[len] = (byte)(adr_my[i]/100);
					data[len+1] = (byte)(adr_my[i]/10)-10*data[len]+(byte)'0';
					data[len] += (byte)'0';
					len += 2;
				}else if(adr_my[i] >= 10){
					data[len] = (byte)(adr_my[i]/10)+(byte)'0';
					len += 1;
				}
				data[len] = (byte)(adr_my[i]%10)+(byte)'0';
				data[len+1] = (byte)'.';		// 最後のデータdata[len]に「.」が入るが
				len += 2;
			}
			len--;								// 最後の「.」は送信しない。
			if( xbee_tx_rx( "RATDL", data ,len ) > 0 ){ 		// 下位４バイトを設定
				dd = 0x00;
			}else{
				#ifdef LCD_H
					xbee_log( 4, "ERR:tx_rx RATDL at set_myadd" , 1 );
				#endif
			}
			// XBee Wi-Fi 2.0だと情報をクラウドに送信してしまう。LAN内に送信するように変更する
			if(DEVICE_TYPE == XB_TYPE_WIFI20){
				data[0]=0x00;	xbee_tx_rx( "RATDO", data ,1 ); // DO: XBee WiFi 2.0用 Device Option=0
			}
		#endif
	}else{
		#ifdef LCD_H
			xbee_log( 4, "ERR:at_my at set_myadd" , 1 );
		#endif
	}
	return(dd);
}

void xbee_from(byte *address){
/*
	最後に受信したデバイスの送信元アドレスの読み込み。
	但し、xbee_rx_callで受信した場合の送信元アドレスは
	「構造体XBEE_RESULT変数.FROM」で読むこと。
	（キャッシュ値での受信の場合があるので）
	出力：byte *address IEEEアドレス
*/
	byte i;

	#ifndef XBEE_WIFI
		for(i=0; i< 8 ;i++ ) address[i] = ADR_FROM[i];
	#else // XBEE_WIFI
		for(i=0; i< 4 ;i++ ) address[i] = ADR_FROM[i];
	#endif
	#ifdef LCD_H
		xbee_log( 1, "done:xbee_from" , 0 );
	#endif
}

byte text2hex4data( byte *data, const char *in , const byte com_len){
/*
	data		出力：変換後の数値データ（バイナリデータ）
	in			入力：文字としての16進数（テキストデータ）
	com_len 	入力：ATコマンドの文字数(ATを含む)。16進数に変換する部分以外の先頭文字数
	戻り値	   出力：変換したバイト数
*/
	byte i, j, len, val;

	/* ATコマンドに続く16進数の引数入力 */
	for( len = 0 ; len < API_SIZE ; len++ ){
		j = (byte)in[len * 2 + com_len];
		i = (byte)in[len * 2 + com_len + 1];
		if( j == (byte)'\0' || i == (byte)'\0' )break;
		// 16進数の文字を数字にしてdataに代入。1桁は考慮。奇数桁は考慮せずに処理。
		if( (j >= (byte)'0' && j <= (byte)'9') || (j >= (byte)'A' && j <= (byte)'F') ){
			if( j > '9' ) data[len] = j - (byte)'A' + 10;
			else data[len] = j - (byte)'0';
			if( (i >= (byte)'0' && i <= (byte)'9') || (i >= (byte)'A' && i <= (byte)'F') ){
				if( i > '9' ) val = i - (byte)'A' + 10;
				else val = i - (byte)'0';
				data[len] *= 16;
				data[len] += val;
			}
		}else break;
	}
	return( len );
}

byte xbee_rat(const byte *address, const char *in){
/*
	リモートATコマンドを送信するユーザ提供関数
	入力：byte *address = 宛先(子機)アドレス
	　　　char *in = 入力するATコマンド 例： ATDL0000FFFF 最大文字数は XB_AT_SIZE-1
	出力：ATコマンドの結果 STATUS_OK / STATUS_ERR / STATUS_ERR_AT / STATUS_ERR_PARM / STATUS_ERR_AIR
	　　　0xFFで送信失敗もしくはデータ異常
*/
	byte i;
	char at[6];
	byte data[API_SIZE];
	byte len;
	byte ret = 0xFF;
	byte com_len =0;
	byte null_adr[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

	if( in[0]=='A' && in[1]=='T' ){
		if( bytecmp(null_adr,address,8) == 0 ){
			// xbee_atから呼ばれた場合 ATxx_;ローカルAT
			com_len = 4;						// 入力ATコマンド長 ATxx 4
			at[0]='A'; at[1]='T';
			if( in[2] == '\0' ){
				at[2]='D'; at[3]='D';
			}else{
				at[2]=in[2]; at[3]=in[3];
			}
			at[4]='\0';
		}else{
			// リモートat(xbee_rat)指定で呼び出された場合 RATxx_；実際のリモートAT
			com_len = 4;						// 入力ATコマンド長 ATxx 4
			at[0]='R'; at[1]='A'; at[2]='T';
			// アプリ呼び出しにつき、ATの後に何もない場合が想定される。その時はATDDとみなす
			if( in[2] == '\0' ){
				at[3]='D'; at[4]='D';
			}else{
				at[3]=in[2]; at[4]=in[3];
			}
			at[5]='\0';
		}
	}else if( in[0]=='R' && in[1]=='A' && in[2]=='T' ){
		com_len = 5;							// 入力ATコマンド長 RATxx 5
		at[0]='R'; at[1]='A'; at[2]='T';
		at[3]=in[3]; at[4]=in[4];
		at[5]='\0';
	}else if( in[0]=='T' && in[1]=='X' ){
		com_len = 2;							// 入力ATコマンド長 TX 2
		at[0]='T'; at[1]='X';
		at[2]='\0';
	}
	#ifndef ARDUINO
	#ifndef ARM_MBED
	else if(in[0]=='Z' && in[1]=='C'){
		com_len = 2;							// 入力ATコマンド長 TX 2
		at[0]='Z'; at[1]='C';
		at[2]='\0';
		if( SADR_DEST[0]==0xFF && SADR_DEST[1] == 0xFE) xbee_address(address);
		len = text2hex4data( data, in , com_len);
		if( xbee_at_tx( at, data ,len ) > 0 ) ret=0;
		com_len = 0;
	}
	#endif
	#endif
	if( com_len > 0 ){
		if( at[0] != 'A' && SADR_DEST[0] == 0xFF && SADR_DEST[1] == 0xFE) xbee_address(address);
		len = text2hex4data( data, in , com_len);
		// test
		// printf("xbee_tx_rx %s len=%d\n",at,len);
		#ifdef LCD_H
			xbee_log( 1, "started:xbee_rat" , len );
		#endif
		if( xbee_tx_rx( at, data ,len ) > 0 ){
			#ifdef LCD_H
				xbee_log( 1, "done:rat" , len );
			#endif
			if( data[3] == MODE_RES ){
				ret = data[7];
			}else if( data[3] == MODE_RESP ){
				for(i=0;i<8;i++) ADR_FROM[i]=data[5+i]; // 2013.9.15 追加
				ret = data[17];
			}else{
				ret = 0xFF;
			}
		}else{
			#ifdef LCD_H
				xbee_log( 4, "ERR:tx_rx at xbee_rat" , 1 );
			#endif
		}
	}
	#ifdef LCD_H
		xbee_log( 1, "done:xbee_rat" , ret );
	#endif
	return( ret );
}

byte xbee_rat_force(const byte *address, const char *in){
/*
	リモートATコマンドを送信するユーザ提供関数。応答値をxbee_rx_callで得る。
	入力：byte *address = 宛先(子機)アドレス
	　　　char *in = 入力するATコマンド 例： ATDL0000FFFF 最大文字数は XB_AT_SIZE-1
	出力：0x00で送信失敗もしくはデータ異常
*/
	char at[6];
	byte data[API_SIZE];
	byte len;
	byte ret = 0x00;

	if( in[0]=='A' && in[1]=='T' ){
		at[0]='S'; at[1]='A'; at[2]='T';
		at[3]=in[3]; at[4]=in[4];
		at[5]='\0';
		xbee_address(address);
		len = text2hex4data( data, in , 5 );
		#ifdef LCD_H
			xbee_log( 1, "started:xbee_ratf" , len );
		#endif
		if( xbee_at_tx( at, data ,len ) > 0 ){
			#ifdef LCD_H
				xbee_log( 1, "done:ratf" , len );
			#endif
			ret = PACKET_ID;
		}
		#ifdef XBEE_WIFI
			if(ret){	// 送信データを即確定する
				at[0]='S'; at[1]='A'; at[2]='T';
				at[3]='A'; at[4]='C';
				xbee_at_tx( at, data ,0 );
			}
		#endif
	}
	#ifdef LCD_H
		xbee_log( 1, "done:xbee_rat_force" , 1 );
	#endif
	return( ret );
}

byte xbee_at(const char *in){
/*
	ローカルATコマンドを送信するユーザ提供関数。
	入力：char *in = 入力するATコマンド 例： ATDL0000FFFF 最大文字数は XB_AT_SIZE-1
	出力：ATコマンドの結果 STATUS_OK / STATUS_ERR / STATUS_ERR_AT / STATUS_ERR_PARM / STATUS_ERR_AIR
	　　　0xFFで送信失敗もしくはデータ異常
	　　　別関数xbee_atnjとエラー時の戻り値が異なる。
	　　　atnjは正常が複数あるので異常時がNULL、atはエラーが複数あるので正常時がNULL
*/
	#ifndef XBEE_WIFI
		byte address[] =	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
		byte ret;
		ret = xbee_rat( address, in);
	#else // XBEE_WIFI
		byte ret;
		ret = xbee_rat( ADR_DEST, in);
	#endif

	#ifdef LCD_H
		if( ret == 0xFF ) xbee_log( 4, "ERR:tx_rx at xbee_at" , ret );
		xbee_log( 1, "done:xbee_at" , 1 );
	#endif
	return( ret );
}

byte xbee_uart(const byte *address, const char *in){
/*
	入力：byte *address = 宛先(子機)アドレス
	　　　char *in = 送信するテキスト文字。最大文字数はAPI_TXSIZE-1
	出力：戻り値 = 送信パケット番号PACKET_ID。0x00は失敗。
*/
	byte ret=0;

	xbee_address( address );						// 宛先のアドレスを設定
	if( xbee_putstr( in ) > 0 ) ret = PACKET_ID;
	#ifdef LCD_H
		xbee_log( 1, "done:xbee_uart" , 1 );
	#endif
	return( ret );
}

byte xbee_bytes(const byte *address, const byte *in, byte len){
/*
	入力：byte *address = 宛先(子機)アドレス
	　　　byte *in = 送信するバイナリ情報。最大文字数はAPI_TXSIZE-1
	　　　byte len = バイト数
	出力：戻り値 = 送信パケット番号PACKET_ID。0x00は失敗。
*/
	byte ret=0;

	#ifdef LCD_H
		xbee_log( 1, "started:xbee_bytes" , 1 );
	#endif
	xbee_address( address );							// 宛先のアドレスを設定
	if(len > API_TXSIZE-18 ){
		#ifdef LCD_H
			xbee_log( 3, "CAUTION:xbee_bytes length" , len );
		#endif
		len = API_TXSIZE-18;
	}
	if( xbee_at_tx( "TX", in , len) > 0 ) ret = PACKET_ID;
	#ifdef LCD_H
		xbee_log( 1, "done:xbee_bytes" , 1 );
	#endif
	return( ret );
}

byte xbee_atvr( void ){
/*
	XBeeのデバイスタイプを取得するコマンド
	出力：戻り値＝ define値 ZB_TYPE_COORD、ZB_TYPE_ROUTER、ZB_TYPE_ENDDEV、0xFF＝エラー
*/
	#ifndef XBEE_WIFI
		byte data[API_SIZE];
		byte ret=0xFF;

		if( xbee_tx_rx( "ATVR", data ,0 ) > 0){
			DEVICE_TYPE = data[8];
			if( DEVICE_TYPE != ZB_TYPE_COORD &&
				DEVICE_TYPE != ZB_TYPE_ROUTER &&
				DEVICE_TYPE != ZB_TYPE_ENDDEV) ret = 0xFF;
			else ret = DEVICE_TYPE;
		}
		#ifdef LCD_H
			xbee_log( 1, "done:xbee_atvr" , ret );
		#endif
		return( ret );
	#else
		return( DEVICE_TYPE );
	#endif
}

byte xbee_atcb( byte cb ){
/*
	XBeeのコミッション制御
	入力：コミッションボタンの押下数（1:ジョイン開始、2:ジョイン許可、4:初期化）
	出力：戻り値＝ 0xFF 異常終了、 0x00 指示成功
*/
	#ifndef XBEE_WIFI
		byte data[API_SIZE];
		byte ret = 0xFF;

		data[0] = cb;
		if( cb == 1 || cb == 2 ){
			if( xbee_tx_rx( "ATCB", data ,1 ) > 0) ret = 0x00;
		}else if( cb == 4){
			if( xbee_tx_rx( "ATCB", data ,1 ) > 0){
				if( DEVICE_TYPE == ZB_TYPE_COORD ){
					wait_millisec(1000);
					sci_clear();
					wait_millisec(5000);
					xbee_at_rx( data );
					if( data[3] == MODE_MODM && data[4] == MODM_STARTED ){
						ret = 0x00;
					}
				}else ret = 0x00;
			}
		}
		#ifdef LCD_H
			xbee_log( 1, "done:xbee_atcb" , ret );
		#endif
		return( ret );
	#else
		return( 0xFF );
	#endif
}

byte xbee_atai(void){
/*
	XBeeモジュールの状態を確認するコマンド　AT Association Indication
	出力：
		0x00 - Successfully formed or joined a network. (Coordinators form a network, routers
				and end devices join a network.)
				※Routerに対して実行すると01が返ってくる場合があるが仕様書には記載なし。
		0x21 - Scan found no PANs
		0x22 - Scan found no valid PANs based on current SC and ID settings
		0x23 - Valid Coordinator or Routers found, but they are not allowing joining (NJ expired)
		0x24 - No joinable beacons were found
		0x25 - Unexpected state, node should not be attempting to join at this time
		0x27 - Node Joining attempt failed (typically due to incompatible security settings)
		0x2A - Coordinator Start attempt failed‘
		0x2B - Checking for an existing coordinator
		0x2C - Attempt to leave the network failed
		0xAB - Attempted to join a device that did not respond.
		0xAC - Secure join error - network security key received unsecured
		0xAD - Secure join error - network security key not received
		0xAF - Secure join error - joining device does not have the right preconfigured link key
		0xFF - Scanning for a ZigBee network (routers and end devices)

注意：本コマンドは応答待ちを行うので干渉によるパケット損失があります。
　　　本コマンドをXBeeに送ると、XBeeからの応答用キャッシュがクリアされる場合があります。
　　　例えば、xbee_forceなどの応答を後から得るコマンドを実行後に本コマンドを実行すると
　　　xbee_rx_callで応答が受け取れなくなる場合があります。
　　　このような場合、xbee_ataiを実行してからxbee_forceを実行するような手順にしてください。
*/
	byte data[API_SIZE];
	byte ret=0xFF;

	#ifdef LCD_H
		xbee_log( 1, "started:xbee_atai" , 0x00 );
	#endif
	if( xbee_tx_rx( "ATAI", data ,0 ) > 0 ){
	//	wait_millisec(100);
		#ifndef XBEE_WIFI
			if( data[7] == 0x00 ) ret = data[8];
		#else
			if( data[17] == 0x00 ) ret = data[18];
		#endif
	}
	#ifdef LCD_H
		xbee_log( 1, "done:xbee_atai" , ret );
	#endif
	return( ret );
}

unsigned short xbee_atop(byte *pan_id){
/*
	PAN Idを求めるコマンド(書き込めないD
	出力：byte pan_id[8] PAN_ID 64bit (8bytes)
	　　　unsigned short 戻り値 = PAN_ID 16bit
注意：本コマンドは応答待ちを行うので干渉によるパケット損失があります。
*/
	#ifndef XBEE_WIFI
		byte data[API_SIZE];
		byte i;
		unsigned short pan_id_s=0xFFFF;

		for( i=0 ; i<8 ; i++ ) pan_id[i] = 0x00;
		#ifdef LCD_H
			xbee_log( 1, "started:xbee_atop" , 0x00 );
		#endif
		if( xbee_tx_rx( "ATOP", data ,0 ) > 0 ){
			wait_millisec(100);
			if( data[7] == 0x00 ){
				for( i=0 ; i<8 ; i++ ) pan_id[i] = data[8+i];
			}
			xbee_tx_rx( "ATOI", data ,0 );
			if( data[7] == 0x00 ){
				pan_id_s = (unsigned short)data[8] * 16 + (unsigned short)data[9];
			}
		//	wait_millisec(100);
		}
		#ifdef LCD_H
			xbee_log( 1, "done:xbee_atop" , 0x00 );
		#endif
		return( pan_id_s );
	#else
		return( 0x00 );
	#endif
}

byte xbee_atee_on(const char *key ){
/*
	暗号化通信の設定(コーディネータ、ルータ共用)
	入力：const char *key セキュリティ共通キー16文字まで
	出力：byte 戻り値 = 0xFF失敗 0x00書き込み 0x01何もせず(設定されていた)
*/
	#ifndef XBEE_WIFI
		byte i;
		byte ret = 0xFF;
		byte data[API_SIZE];

		/* 現在のモードを確認(あとでATWRする。フラッシュ書き換え回数低減のため) */
	//	xbee_tx_rx( "ATVR", data ,0 );
	//	vr = data[8];			// xbee VRを取得
		if( xbee_tx_rx( "ATEE", data ,0 ) > 0 ){
			if( data[8] == 0x00 ){	// ATEEの応答が00の時
				data[0] = 0x01; 				// ATEE = 0x01に設定
				if( xbee_tx_rx( "ATEE", data ,1 ) > 0 ){
					if( DEVICE_TYPE == ZB_TYPE_COORD ){
						wait_millisec(1000);
						sci_clear();
						wait_millisec(5000);
						xbee_at_rx( data );
						if( data[3] == MODE_MODM && data[4] == MODM_STARTED ){
							data[0] = 0x00; 	// ATNK = 0x00 random network key
							ret = xbee_tx_rx( "ATNK", data ,1 );
						}
					}else ret = 0x01;
					/* KY 設定 */
					for(i=0;i<16;i++){
						if( key[i] == '\0' ) break;
						data[i] = (byte)key[i];
					} // ここで使ったiはキーの文字数として使用する
					if( i == 0 ){
						#ifdef LCD_H
							xbee_log( 3, "CAUTION:ATKY=" , 0 );
						#endif
						data[0] = 0x00;
						i = 1;
					}
					if( xbee_tx_rx( "ATKY", data ,i ) > 0 ){
						if( xbee_tx_rx( "ATWR", data ,0 ) > 0 ){
							if( ret != 0xFF ) ret = 0x00;
						}
					}
				}
			}else ret = 0x01;
		}
		#ifdef LCD_H
			xbee_log( 1, "done:xbee_atee_on" , ret );
		#endif
		return(ret);
	#else
		return( 0x00 );
	#endif
}

byte xbee_atee_off( void ){
/*
	暗号化通信のオフ(コーディネータ、ルータ共用)
	出力：byte 戻り値 = 0xFF失敗 0x00書き込み 0x01何もせず(設定されていた)
*/
	#ifndef XBEE_WIFI
		byte data[API_SIZE];
		byte ret = 0xFF;

		if( xbee_tx_rx( "ATEE", data ,0 ) > 0 ){
			if( data[8] == 0x01 ){	// ATEEの応答が01(enable)の時
				data[0] = 0x00; 				// ATEE = 0x00に設定
				if( xbee_tx_rx( "ATEE", data ,1 ) > 0 ){
					if( DEVICE_TYPE == ZB_TYPE_COORD ){ // ZigBeeコーディネータの場合は再起動がある
						wait_millisec(1000);
						sci_clear();
						wait_millisec(5000);
						xbee_at_rx( data );
						if( data[3] == MODE_MODM && data[4] == MODM_STARTED ){
							data[0] = 0x00; 	// ATNK = 0x00 random network key
							ret = xbee_tx_rx( "ATNK", data ,1 );
						}
					}else ret = 0x01;
					if( xbee_tx_rx( "ATWR", data ,0 ) > 0 ){
						if( ret != 0xFF ) ret = 0x00;
					}
				}
			}else ret= 0x01;
		}
		#ifdef LCD_H
			xbee_log( 1, "done:xbee_atee_off" , ret );
		#endif
		return(ret);
	#else
		return( 0x00 );
	#endif
}

byte xbee_atnc(void){
/*
	XBeeモジュールのEnd Device子機のジョイン可能な台数を取得するコマンドです。
	出力：ジョイン可能数(0～)、0xFFで取得エラー

注意：本コマンドは応答待ちを行うので干渉によるパケット損失があります。
*/
	#ifndef XBEE_WIFI
		byte data[API_SIZE];
		byte ret=0xFF;
		if( xbee_tx_rx( "ATNC", data ,0 ) > 0 ){
			if( data[7] == 0x00 ) ret = data[8];
		}
		#ifdef LCD_H
			if(ret<3) xbee_log( 3, "CAUTION:Many Children" , ret );
			if(ret==0) xbee_log( 3, "CAUTION:Cannot Join More" , ret );
			xbee_log( 1, "done:xbee_atnc" , ret );
		#endif
		return( ret );
	#else
		return( 0 );
	#endif
}

byte xbee_ratnc( const byte *address ){
/*
	XBeeモジュールのEnd Device子機のジョイン可能な台数を取得するコマンドです。
	出力：ジョイン可能数(0～)、0xFFで取得エラー

注意：本コマンドは応答待ちを行うので干渉によるパケット損失があります。
　　　本コマンドをXBeeに送ると、XBeeからの応答用キャッシュがクリアされる場合があります。
*/
	#ifndef XBEE_WIFI
		byte data[API_SIZE];
		byte ret=0xFF;

		xbee_address(address);							// 宛先のアドレスを設定
		if( xbee_tx_rx( "RATNC", data ,0 ) > 0 ){
			if( data[17] == 0x00 ) ret = data[18];
		}
		#ifdef LCD_H
			if(ret<3) xbee_log( 3, "CAUTION:Many Children" , ret );
			xbee_log( 1, "done:xbee_ratnc" , ret );
		#endif
		return( ret );
	#else
		return( 0 );
	#endif
}

byte xbee_atnj(const byte timeout){
/*
	制限時間timeoutで設定した間にネットワークに参加したデバイスを通信相手に設定する関数
	AT Node Join Time
	入力：timeout = 時間(6～254秒)、0x00で参加拒否、0xFFの場合は常に参加許可
	出力：byte 戻り値 = 0x00失敗、成功時はMODE_IDNT(0x95)が代入される
	　　　xbee_atとエラー時の戻り値が異なる。
	　　　atnjは正常が複数あるので異常時がNULL
	　　　また、atnjはfopenと似た記述になるので、特殊な応答でもない(と思う)

注意：本コマンドは応答待ちを行うので干渉によるパケット損失があります。
*/
	#ifndef XBEE_WIFI
		byte data[API_SIZE];
		byte i=0, ret, timer;

		#ifdef LCD_H
			xbee_log( 1, "started:xbee_atnj" , timeout );
		#endif
		data[0]=timeout;
		ret = xbee_tx_rx( "ATNJ", data ,1 );
		if( ret > 0 ){
			wait_millisec(100);
			if( timeout == 0xFF ){
				/*常に参加待ち受けするテスト専用(非実用)*/
				ret=0xFF;
				#ifdef LCD_H
					xbee_log( 3, "CAUTION:ATNJ=" , timeout );
				#endif
				wait_millisec(1000);			// NJ値の書込み時間待ち
			}else{
				if( timeout > 0x09 ){
					/*参加待ち受けの開始*/
					timer = TIMER_SEC + timeout + 0x01 ;
					ret = 0x00;
					while( timer != TIMER_SEC && ret != MODE_IDNT){ // MODE_IDNT(0x95)はネットワーク参加
						#ifdef H3694
							if( (timera()>>6)&0x01 ){
								led_green( 0 ); // LEDのトグル1秒に4回
							} else {
								led_green( 1 );
							}
						#endif
						if( xbee_at_rx( data ) > 0x00 ){	// データの受信(戻り値は受信種別mode値)
							ret = xbee_from_acum(data);
							if( ret == MODE_IDNT ){ 	// 受信モードがMODE_IDNT(0x95) Node Identifyの場合
								#ifdef LCD_H
									xbee_log( 1, "Found device " , ret );
								#endif
								for(i=0;i<8;i++) ADR_DEST[i]=ADR_FROM[i];			// 宛先を発見デバイスに設定
								#ifndef LITE	// BeeBee Lite by 蘭
									sci_write_check();
								#endif
								sci_clear();
								}else ret=0x00;
						}
					}
					#ifdef LCD_H
						if( ret == 0x00 ) xbee_log( 1, "no new device" , ret );
					#endif
				}
				/*ジョイン拒否設定*/
				data[0]=0x00;
				xbee_tx_rx( "ATNJ", data ,1 );
				#ifdef LCD_H
					xbee_log( 1, "done:atnj=0x00" , 0x00 );
				#endif
			}
		}else{
			#ifdef LCD_H
				xbee_log( 5, "ERROR:xbee_atnj" , ret );
			#endif
		}
		#ifdef LCD_H
			xbee_log( 1, "done:xbee_atnj" , ret );
		#endif
		return( ret );
	#else
		return( 0x95 );
	#endif
}

byte xbee_ratnj(const byte *address, const byte timeout){
/*
	前記xbee_atnjをリモート先(子機)に対して設定する関数
	入力：byte *address = 宛先(子機)アドレス
	入力：timeout = 0でジョイン拒否、1～254で待ち秒数、255で常時ジョイン許可
	出力：byte 戻り値 = 0x00失敗、0xFF成功

注意：本コマンドは応答待ちを行うので干渉によるパケット損失があります。
*/
	#ifndef XBEE_WIFI
		byte data[API_SIZE];
		byte ret=0x00;

		xbee_address(address);							// 宛先のアドレスを設定
		data[0]=timeout;
		if( xbee_tx_rx( "RATNJ", data ,1 ) > 0 ){
			#ifdef LCD_H
				xbee_log( 1, "done:ratnj" , xbee_tx_rx( "ATAC", data ,0 ) );
			#endif
			ret=0xFF;
		}
		#ifdef LCD_H
			xbee_log( 1, "done:xbee_ratnj" , ret );
		#endif
		return( ret );
	#else
		return( 0x95 );
	#endif
}

byte xbee_ping(const byte *address ){
/*
入力：IEEEアドレスaddress
出力：				   0xFF 	   エラー
		DEV_TYPE_XBEE	0x00		// XBeeモジュール
		DEV_TYPE_RS232	0x05		// RS-232Cアダプタ
		DEV_TYPE_SENS	0x07		// Sensor (1wire専用)
		DEV_TYPE_PLUG	0x0F		// SmartPlug

	byte 戻り値 = 0xFF失敗、成功時は0x00などのデバイス名(ATDD)の下2桁

注意：本コマンドは応答待ちを行うので干渉によるパケット損失があります。
*/
	byte i;
	byte ret=0xFF;		// デバイス名
	byte data[API_SIZE];

	xbee_address(address);							// 宛先のアドレスを設定
	if( xbee_tx_rx( "RATDD", data ,0 ) > 0 ){		// デバイス名を取得
		#ifdef XBEE_WIFI			// Device Type Identifier. Stores a device type value
									// 20 00 20 24 ← 電源投入した後は、こんな応答がある。モジュールによらず。
									// 00 90 00 00		仕様書に書かれていないと思う
		if( (data[18]==0x00 && data[19]==0x09 && data[20]==0x00 && data[21]==0x00) ||
			(data[18]==0x20 && data[19]==0x00 && data[20]==0x20 && data[21]==0x24) ){	// XBee WiFiデバイス
			data[21]=0x00;
		#else
		if( data[18]==0x00 && data[19]==0x03 && data[20]==0x00 ){	// XBeeデバイス
		#endif
			for(i=0;i<8;i++) ADR_FROM[i]=data[5+i]; // 2013.9.15 追加
			ret=data[21];							// デバイス名をddに代入
		}
		#ifdef DEBUG
			#ifdef ARM_MBED
				for(i=18;i<=21;i++) _xbee_debug.printf("%2X ",data[i]);
				_xbee_debug.printf("\n");
			#else //PC
				for(i=18;i<=21;i++) printf("%2X ",data[i]);
				printf("\n");
			#endif
		#endif
	}
	#ifdef LCD_H
		xbee_log( 1, "done:xbee_ping" , ret );
	#endif
	#ifdef XBEE_WIFI_DEBUG
		Serial.print("DD=");
		for(i=18;i<=21;i++){
			Serial.print(data[i],HEX);
			Serial.print(" ");
		}
		Serial.println("");
	#endif
	return( ret );
}

int xbee_batt(const byte *address ){
/*
電池電圧の確認
入力：IEEEアドレスaddress
出力：約2100～3600[mV]、-1=エラー
注意：本コマンドは応答待ちを行うので干渉によるパケット損失があります。
*/
	int ret=-1;
	byte data[API_SIZE];

	xbee_address(address);							// 宛先のアドレスを設定
	if( xbee_tx_rx( "RAT%V", data ,0 ) > 0 ){		// 電池電圧を取得
		ret = (int)(((float)data[18] * 256.0f + (float)data[19] ) * 1.17f);
	}
	#ifdef LCD_H
		xbee_log( 1, "done:xbee_batt" , (byte)(ret/100) );
	#endif
	return( ret );
}

byte xbee_batt_force(const byte *address ){
/*
電池電圧の確認
入力：IEEEアドレスaddress
xbee_batt_forceで測定指示を出して、xbee_rx_callで受け取ります。
XBEE_RESULT.MODEに「MODE_BATT(0xE1)」が設定されるので他のリモートＡＴレスポンスと区別することが出来ます。
電圧値はXBEE_RESULT.ADC[0]に約2100～3600[mV]の範囲で応答します。
*/
	byte data[XB_AT_SIZE];
	byte ret = 0x00;

	xbee_address( address );							// 送信先を指定
	ret = xbee_at_tx("RAT%V", data , 0);				// ATISを送信
	if( ret > 0 ) ret = PACKET_ID;
	#ifdef LCD_H
		xbee_log( 1, "done:xbee_batt_force" , ret );
	#endif
	return ( ret );
}

#ifdef ARDUINO
byte xbee_gpio_config(const byte *address, const byte port, byte type ){
#else
#ifdef ARM_MBED
byte xbee_gpio_config(const byte *address, const byte port, byte type ){
#else
byte xbee_gpio_config(const byte *address, const byte port, const enum xbee_port_type type ){
#endif
#endif
/*
入力：IEEEアドレスaddress、GPIOポート番号port、設定値type
		enum xbee_port_type{ DISABLE=0, VENDER=1, AIN=2, DIN=3, DOUT_L=4, DOUT_H=5 };

	port:	port指定  IO名 ピン番号    共用 	 USB評価ボード(XBIB-U-Dev)
			port= 0 	DIO0	XBee_pin 20 (Cms)		SW Commision コミッションボタン
			port= 1 	DIO1	XBee_pin 19 (AD1)		SW2 汎用入力用(DIN or AIN)
			port= 2 	DIO2	XBee_pin 18 (AD2)		SW3 汎用入力用(DIN or AIN)
			port= 3 	DIO3	XBee_pin 17 (AD3)		SW4 汎用入力用(DIN or AIN)
			port= 4 	DIO4	XBee_pin 16 (RTS)		LED3 汎用出力用
			port= 5 	DIO5	XBee_pin 15 (Ass)		ASSOSIATE indication
			port=10 	DIO10	XBee_pin  6 (RSSI)		RSSI indication (PWM)
			port=11 	DIO11	XBee_pin  7 			LED2 汎用出力用
			port=12 	DIO12	XBee_pin  4 			LED1 汎用出力用

注意：本コマンドは応答待ちを行うので干渉によるパケット損失があります。
*/
	byte ret=0xFF;
	byte data[API_SIZE];
	char at[] = "RATD0" ;	// ATコマンド
	byte dd=0xFF;			// デバイス名

	switch( type ){
		case DISABLE:
			if( (port >= 1 && port <= 5) || (port >= 10 && port <= 12) ) {
				at[4] = '0' + port;
				ret=0;
			}
			break;
		case VENDER:
			if( port == 0 || port == 5 || port == 10) ret=0;
			break;
		case AIN:
			if( port >= 1 && port <= 4 ){
				at[4] = '0' + port;
				ret=0;
			}else if( port == 0xFF ){
				ret=0;
			}
			break;
		case DIN:
		case DOUT_L:
		case DOUT_H:
			if( port >= 1 && port <= 5 ){
				at[4] = '0' + port;
				ret=0;
			}else if( port >= 10 && port <= 12 ){
				at[3] = 'P';
				at[4] = '0' + (port - 10);
				ret=0;
			}else if( port == 0xFF ){
				ret=0;
			}
			break;
		default:
			break;
	}
	if( ret==0 ){
		dd = xbee_ratd_myaddress(address);
		if( dd != 0xFF ){					// アドレス設定が正常に実施された時
			/*
			入力されたtypeがDINの時にRATICを変更する処理に対応予定（将来）
			RATICで状態を取得して状態or設定を追加(ポート1～5と10～12)
			*/
			data[0]=(byte)type;
			xbee_tx_rx( at , data ,1 );
		}
	}
	#ifdef LCD_H
		xbee_log( 1, "done:xbee_gpio_config" , dd );
	#endif
	return( dd );
}

byte xbee_gpio_init(const byte *address){
/*
子機XBeeデバイス(リモート先)のGPIOを入力に設定する関数
リモート端末の宛先を本機アドレスに設定(リモート側のボタンで親機に送信するために)
入力：なし
出力：デバイス名(ATDDの下2桁)、FFでエラー
注意：本コマンドは応答待ちを行うので干渉によるパケット損失があります。
*/
	byte data[API_SIZE];
	byte dd;				// デバイス名

	dd = xbee_ratd_myaddress(address);
	if( dd != 0xFF){
		data[0]=0x01;		xbee_tx_rx( "RATD0", data ,1 ); // ポート0をコミッションボタンに設定
		switch(dd){
			case DEV_TYPE_XBEE:
				data[0]=0x03;	xbee_tx_rx( "RATD1", data ,1 ); // ポート1をデジタル入力に設定
				data[0]=0x03;	xbee_tx_rx( "RATD2", data ,1 ); // ポート2をデジタル入力に設定
				data[0]=0x03;	xbee_tx_rx( "RATD3", data ,1 ); // ポート3をデジタル入力に設定
				data[0]=0x05;	xbee_tx_rx( "RATD4", data ,1 ); // ポート4をデジタル出力に設定
				data[0]=0x05;	xbee_tx_rx( "RATP1", data ,1 ); // ポート11をデジタル出力に設定
				data[0]=0x05;	xbee_tx_rx( "RATP2", data ,1 ); // ポート12をデジタル出力に設定
				data[0]=0x00;
				data[1]=0x0E;					// ポート1～3の↓
				xbee_tx_rx( "RATIC", data ,2 ); // 入力値が変化したら通知するIC設定
				break;
			case DEV_TYPE_PLUG:
				data[0]=0x02;	xbee_tx_rx( "RATD3", data ,1 ); // ポート3をアナログ入力に設定
				data[0]=0x04;	xbee_tx_rx( "RATD4", data ,1 ); // ポート4をデジタル出力に設定
				// breakしない
			case DEV_TYPE_WALL:
				data[0]=0x02;	xbee_tx_rx( "RATD1", data ,1 ); // ポート1をアナログ入力に設定
				data[0]=0x02;	xbee_tx_rx( "RATD2", data ,1 ); // ポート2をアナログ入力に設定
				break;
			default:
				#ifdef LCD_H
					xbee_log( 4, "ERR:gpio_init DD" , dd );
				#endif
				break;
		}
	}
	#ifdef LCD_H
		xbee_log( 1, "done:gpio init" , dd );
	#endif
	return( dd );
}

byte xbee_gpo(const byte *address, const byte port,const byte out){
/*
子機XBeeデバイス(リモート先)のGPIOへ出力する関数
portは1～4と11～12が指定できる→port0～7と10～12に拡大
出力：byte 戻り値 = 送信パケット番号PACKET_ID。0x00は失敗。
	port:	port指定  IO名 ピン番号 		   USB評価ボード(XBIB-U-Dev)
			port= 0 	DIO0	XBee_pin 20 (Cms)		SW1 Commision
			port= 1 	DIO1	XBee_pin 19 (AD1)		SW2
			port= 2 	DIO2	XBee_pin 18 (AD2)		SW3
			port= 3 	DIO3	XBee_pin 17 (AD3)		SW4
			port= 4 	DIO4	XBee_pin 11 			LED3	※port4 がLED3
			port=10 	DIO10	XBee_pin  6 			RSSI
			port=11 	DIO11	XBee_pin  7 			LED2	※port11がLED2
			port=12 	DIO12	XBee_pin  4 			LED1	※port12がLED1
*/
	char at[6];
	byte data[API_SIZE];
//	byte i=0;
	byte ret=0;

	xbee_address(address);									// 宛先のアドレスを設定
	at[0]='S'; at[1]='A'; at[2]='T'; at[3]=0; at[5]='\0';
	// at[0]='R';	// RAT(応答待ち)の場合は本行のコメントを外してRATに変更する（かなり遅くなる）
	if( port <= 7 ){	// port >= 0 &&
		at[3] = 'D';
		at[4] = '0' + port;
	}else if( port >= 10 && port <= 12 ){
		at[3] = 'P';
		at[4] = '0' + (port - 10);
	}
	if( at[3] ){
		data[0]=0x05;					// output = H
		if( out == 0 ) data[0]=0x04;	// output = L
		data[1]=0x00;
		ret = xbee_at_tx( at, data ,1 );
		if( ret > 0 ) ret = PACKET_ID;
		// ret = xbee_tx_rx( at, data ,1 ); // RAT(応答待ち)の場合にコメントを外す
		#ifdef XBEE_WIFI
			if( DEVICE_TYPE == XB_TYPE_WIFI20) delay(10);
					// S6Bで安定動作させるために必要（理由は不明）
			/*
			XBee Wi-Fiの場合はPACKET IDを0にしても応答がある。
			したがってSATでは応答なしATコマンドを明示して送信している。
			ところが、その場合ACを受信するまでコマンドが確定しない。
			その対策としてATACを追加で送信する。
			XBee ZBでもATACで応答遅延が無くなる利点があるが、パケット増加で
			かえって高速動作が出来なくなるので、Wi-Fiのみの対応とする。
			*/
			if(ret){	// 送信データを即確定する
				at[0]='S'; at[1]='A'; at[2]='T';
				at[3]='A'; at[4]='C';
				xbee_at_tx( at, data ,0 );
			}
		#endif
	}
	#ifdef DEBUG
		lcd_goto(LCD_ROW_3);
		if( ret==0 ){
			lcd_putstr("GPO ERR:");
		}else{
			lcd_putstr("GPO OK:");
		}
		lcd_disp_hex( data[17] );
	#endif
	#ifdef LCD_H
		xbee_log( 1, "done:xbee_gpo" , ret );
	#endif
	return(ret);
}

#ifdef EASY_GPI
byte xbee_gpi(const byte *address, const byte port){
/*	リモート先(ADR_FROMに入力)のGPIOの入力値を応答するAPI

★注意：本コマンドは応答待ちを行うので干渉によるパケット損失があります。
本コマンドでは無くxbee_forceとxbee_rx_callの組み合わせでの利用を推奨します。

portは1～4と11～12と0xFFが指定できる。
port=0xFFでポート0～7をHEXで応答する。
AINはxbee_adcを使用する。
	port:	port指定  IO名 ピン番号 		   USB評価ボード(XBIB-U-Dev)
			port= 1 	DIO1	XBee_pin 19 (AD1)		 SW2
			port= 2 	DIO2	XBee_pin 18 (AD2)		 SW3
			port= 3 	DIO3	XBee_pin 17 (AD3)		 SW4
	out: 指定ポートの状態 0または1、エラー時は0xFF

設計情報
	リターン信号例
	00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
	7E 00 15 97 01 00 13 A2 00 -- -- -- -- -- -- 49 53 00 01 18 1E 00 00 0E 8A
	  | len |mo|FI|送信元IEEEアドレス	  |Short|AT(IS)  |01|MASK |AD|DATA |CS|
	  | 	|de|
	  | 	|__|__________________ MODE_RESP(0x97)= ATIS応答
	  |_____|_____________________ Data Length
*/
	byte data[API_SIZE];
	byte ret=0xFF;

	xbee_address(address);								// 宛先のアドレスを設定
	if( xbee_tx_rx("RATIS", data , 0) > 0 ){
	//	if( data[3]==MODE_RESP ){						// d[3] flame ID = 97(RAT応答)
														// tx_rxで data[3]はチェック済み
			if( port == 0xFF) ret = (data[23] & data[20]);
			else{
				if( (port >= 1) && (port <= 4) ){
					if( ((data[20]>>port)&0x01) == 0x00 ){	// MASK_L(20)の該当ビットが否の時
						data[0]=0x03; // リモート端末のポート1～3をデジタル入力(0x03)に設定
						if( port ==1 ) xbee_tx_rx( "RATD1", data ,1 );
						if( port ==2 ) xbee_tx_rx( "RATD2", data ,1 );
						if( port ==3 ) xbee_tx_rx( "RATD3", data ,1 );
						if( port ==4 ) xbee_tx_rx( "RATD4", data ,1 );
						wait_millisec(200);
						xbee_tx_rx("RATIS", data , 0);	// 再度ISを実行
					}
					if( data[3]==MODE_RESP ) ret= ((data[23] >> port ) & 0x01); // 取得データDATA_L(23)を戻り値に
				} else if( (port >= 11) && (port <= 12) ){
					if( ((data[19]>>(port-8))&0x01) == 0x00 ){	// MASK_H(19)の該当ビットが否の時
						data[0]=0x03; // リモート端末のポート11～12をデジタル入力(0x03)に設定
						if( port ==11 ) xbee_tx_rx( "RATP1", data ,1 );
						if( port ==12 ) xbee_tx_rx( "RATP2", data ,1 );
						wait_millisec(200);
						xbee_tx_rx("RATIS", data , 0);	// 再度ISを実行
					}
					if( data[3]==MODE_RESP ) ret= ((data[22] >> (port-8))& 0x01);	// 取得データDATA_H(22)を戻り値に
				}
			}
	//	}
	}
	#ifdef DEBUG
		lcd_goto(LCD_ROW_3);
		lcd_putstr("port"); lcd_disp_hex( port ); lcd_putstr("="); lcd_disp_hex( ret ); // 戻り値
		lcd_goto(LCD_ROW_4);
		lcd_putstr(" f:"); lcd_disp_hex( data[3]);
		lcd_putstr(" M:"); lcd_disp_hex( data[19] ); lcd_disp_hex( data[20] );	// MASK表示
		lcd_putstr(" D:"); lcd_disp_hex( data[22] ); lcd_disp_hex( data[23] );	// デジタルサンプル値
	#endif
	#ifdef LCD_H
		xbee_log( 1, "done:xbee_gpi" , ret );
	#endif
	return( ret );
}
#endif // EASY_GPI

#ifdef EASY_ADC
unsigned int xbee_adc(const byte *address, const byte port){
/*	リモート先(addressに入力)のADCの入力値を応答するAPI
portは1～3が指定できる。指定したportがADC入力でない場合はADC入力に切り替える。
	port:	port指定  IO名 ピン番号 		   USB評価ボード(XBIB-U-Dev)
			port= 1 	DIO1	XBee_pin 19 (AD1)		SW2
			port= 2 	DIO2	XBee_pin 18 (AD2)		SW3
			port= 3 	DIO3	XBee_pin 17 (AD3)		SW4
	out:	ADC値

設計情報
	リターン信号例
	00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26
	7E 00 17 97 01 00 13 A2 00 -- -- -- -- -- -- 49 53 00 01 18 16 08 18 06 00 00 7A
	  | len |mo|FI|送信元IEEEアドレス	  |Short|AT(IS)  |01|GPIO |AD|GPIO |ADC  |CS
	  | 	|de|  | 					  |Adrss|		 |01|MASK |MS|DATA |DATA |
	  | 	|  |  | 											  |  |
	  | 	|  |						  ADC MASK________________|__|
	  | 	|__|__________________ MODE_RESP(0x97)= ATIS応答
	  |_____|_____________________ Data Length

注意：本コマンドは応答待ちを行うので干渉によるパケット損失があります。
*/
	byte data[API_SIZE];
	byte ports=1;
	unsigned int ret=0xFFFF;

	xbee_address(address);								// 宛先のアドレスを設定
	if( xbee_tx_rx("RATIS", data , 0) > 0 ){
		if( data[3]==MODE_RESP ){						// d[3] flame ID = 97(MODE_RESP)
			if( (port >= 1) && (port <= 3) ){
				if( ((data[21]>>port ) & 0x01) == 0x00 ){	// MASKの該当ビットが否の時
					data[0]=0x02; // リモート端末のポート1～3をADC入力(0x02)に設定
					if( port == 1 ) xbee_tx_rx( "RATD1", data ,1 );
					if( port == 2 ) xbee_tx_rx( "RATD2", data ,1 );
					if( port == 3 ) xbee_tx_rx( "RATD3", data ,1 );
					wait_millisec(1000);
					xbee_tx_rx("RATIS", data , 0);	// 再度ISを実行
				}
				if( (port == 2) && ((data[21]>>1)&0x01) ) ports =2; 	// port2指定でport1がADCならデータは2個
				else if ( port == 3 ){
					if( (data[21]>>2)&(data[21]>>1)&0x01 ) ports =3; // port3指定でport1と2がADCならデータは3個
					else if( ((data[21]>>2)|(data[21]>>1))&0x01 ) ports =2; // port3指定でport1か2の片方がADCならデータは2個
				}
				if( data[3]==MODE_RESP ) ret= (unsigned int)data[22+ports*2]*256 + (unsigned int)data[23+ports*2];
																		// 取得データを戻り値に
			}
		}
	}
	#ifdef DEBUG
		lcd_goto(LCD_ROW_4); lcd_putstr("ADC "); lcd_disp_hex( port ); lcd_putstr("=");
		lcd_disp_hex( data[21] ); lcd_putstr(" "); lcd_disp_5( ret );
	#endif
	#ifdef LCD_H
		xbee_log( 1, "done:xbee_adc" , ret );
	#endif
	return( ret );
}
#endif // EASY_ADC

byte xbee_force(const byte *address ){
/*
	GPIOやXBee Sensor(XBee Sensor /L/T または /L/T/H) へ測定指示(ATIS)を送るコマンド
	本xbee_forceで測定指示を出して、xbee_rx_callで受け取ることを推奨します。
	XBee Sensor はxbee_sensor_resultでセンサー値を計算できます。
	入力：送信アドレス
	出力：送信結果（送信したAPIサービス長。送信しなかった場合は0）
*/
	byte ret = 0x00;
	byte null_data [] = { 0x00 };

	xbee_address( address );							// 送信先を指定
	ret = xbee_at_tx("RATIS", null_data , 0);				// ATISを送信
	if( ret > 0 ) ret = PACKET_ID;
	#ifdef LCD_H
		xbee_log( 1, "done:xbee_force" , ret );
	#endif
	return ( ret );
}

#ifdef ARDUINO
float xbee_sensor_result( XBEE_RESULT *xbee_result, const byte type){
#else
#ifdef ARM_MBED
float xbee_sensor_result( XBEE_RESULT *xbee_result, const byte type){
#else
float xbee_sensor_result( XBEE_RESULT *xbee_result, const enum xbee_sensor_type type ){
#endif
#endif
/*
xbee_rx_callで取得したxbee_sensorデータを値に変換するAPI(非通信)
call_result情報から変換
LIGHTはXBee Sensor用
WATTはXBee Smart Plug用

	入力：enum xbee_sensor_type {LIGHT,TEMP,HUMIDITY,WATT};    // センサタイプの型
	出力：float センサ読み取り値

	XBee Sensor(XBee Sensor /L/T または /L/T/H) ※ 1-wire Sensor Adapterではない

	設計情報
		リターン信号例
		00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
		7E 00 1B 97 01 00 13 A2 00 -- -- -- -- -- -- 49 53 00 01 08 00 0E 08 00 03 E2 02 62 02 16 A1
		  | len |mo|FI|送信元IEEEアドレス	  |Short|AT(IS)  |01|GPIO |AD|GPIO |ADC1 |ADC2 |ADC3 |CS
		  | 	|de|  | 					  |Adrss|		 |01|MASK |MS|DATA |DATA |DATA |DATA |
		  | 	|  |  | 											  |  |	   |Light|Temp |Humidity
		  | 	|  |						  ADC MASK________________|__|
		  | 	|__|__________________ MODE_RESP(0x97)= ATIS応答		  [手間！]この先はMASKによって
		  |_____|_____________________ Data Length						  データの無い部分は省略される
*/

	float ret = -1.0f;

	switch(type){
		case LIGHT:
			#ifndef XBEE_WIFI
				ret = ((float)(xbee_result->ADCIN[1]))*1.173f;
													// (ADC1) /1023.0) * 1200
			#else
				ret = ((float)(xbee_result->ADCIN[1]))*2.444f;
													// (ADC1) /1023.0) * 2500
			#endif
			break;
		case TEMP:
			#ifndef XBEE_WIFI
				ret = ((float)(xbee_result->ADCIN[2]))*0.1173f-50.0f;
													// (((ADC2/1023.0) * 1200) - 500.0)/ 10.0
			#else
				ret = ((float)(xbee_result->ADCIN[2]))*0.2444f-50.0f;
			#endif
			/*
			XBee ルータの温度測定結果は内部温度による上昇分が加算されます。
			Digi社によると約4℃の上昇があるので4℃を減算して使用することを推奨している。
			本ライブラリでは減算していませんが、実際の温度差を測定したところ
			平均7.12℃の差がありました。(27.2℃～28.0℃の環境下での平均誤差)
			*/
			break;
		case HUMIDITY:
			#ifndef XBEE_WIFI
				ret = ((float)(xbee_result->ADCIN[3]))*1.173f;
													// mVanalog = (ADC3/1023.0) * 1200
			#else
				ret = ((float)(xbee_result->ADCIN[3]))*2.444f;
			#endif
			ret = ( ((ret * 108.2f / 33.2f) / 5000.0f - 0.16f) / 0.0062f );
													// hum = (((mVanalog * 108.2 / 33.2) / 5000 - 0.16) / 0.0062)
			break;
		case WATT:
			#ifndef XBEE_WIFI
				ret = ((float)(xbee_result->ADCIN[3]))*1.17302f;
			#else
				ret = ((float)(xbee_result->ADCIN[3]))*2.44379f;
			#endif
			ret = (ret * 156.0f / 47.0f - 520.0f) / 180.0f * 70.71f ;
			if( ret < 0 ) ret = 0;
													// current = (mV*(156.0/47.0) - 520.0) /180.0 * .7071
			/*	電圧を測定していないのでワットへの変換は100Vを乗算している。*/
			/*	XBee Smart Plugは誘導負荷が考慮されないので測定値は参考値にしかならない。
				(モーターなどの力率の悪いものは測れない)
				The XBee Smart Plug uses a hall-effect current sensor IC to detect RMS current draw
				from the user controlled outlet. As there is no power factor correction for inductive loads,
				sensor accuracy is only specified for resistive loads. Current measurements can still be
				made on inductive loads that fall within the product load rating, but accuracy is not
				guaranteed. */
			break;
		default:
			break;
	}
	#ifdef LCD_H
		xbee_log( 1, "done:xbee_sensor_result" , (byte)ret );
	#endif
	return( ret );
}

unsigned int xbee_batt_acum(byte a1, byte a2  ){
	return( (unsigned int)( ( (float)a1 * 256.0f + (float)a2 ) * 1.17f ) );
}

byte xbee_rx_call( XBEE_RESULT *xbee_result ){
/*
	XBeeからの受信を行うための待ち受け関数
	Ver 1.70以降はモード指定での待ち受け機能を削除した。
	mode MODE_AUTO	0x00	// 自動選択(modeに選択後のモード名が入る)
	mode MODE_UART	0x90	// UART Receive
	mode MODE_UAR2	0x91	// UART AO=0
	mode MODE_GPIN	0x92	// GPI data
	mode MODE_SENS	0x94	// XB Sensor
	mode MODE_IDNT	0x95	// Node Identify
	mode MODE_RES	0x88	// ローカルATコマンドの結果を受信
	mode MODE_RESP	0x97	// リモートATコマンドの結果を受信
	mode MODE_MODM	0x8A	// Modem Statusを受信
	mode MODE_TXST	0x8B	// UART Transmit Status を受信
	mode MODE_BATT	0xE1	// (独自定義)バッテリステータス RAT%Vの応答時

	出力　　　：byte データ
	入力・出力：byte *mode モード値
	出力　　　：byte *xbee_result データの戻り値
	　　　　　　GPIデータ(無いGPIデータは0になるADCはFFFFになる)
	　　　　　　   00 01 02 03 04 05 06 07 08 09 0A 0B 0C ....
	　　　　　　  |GPI	|ADC0 |ADC1 |ADC2 |ADC3 |\0|\0|\0|....
	　　　　　　UARTデータは最後が\0で戻り値がlength
*/
	byte data[API_SIZE];
	byte ret=0;
	byte return_MODE = 0x00;
	byte cache_en = 0x00;
	byte i, j;

	// 各種APIによるエラー発生の解除処理
	#ifdef ERRLOG
		if( TIMER_ERR ){
			#ifdef LCD_H
				xbee_log( 5, ERR_LOG , ERR_CODE );	// エラーログを出力
			#endif
			xbee_reset();							// リセットに約3秒がかかる
			#ifdef LCD_H
				xbee_log( 3, "XBee Restarted" , TIMER_ERR );
			#endif
			#ifdef H3694
				led_red( 0 );
			#endif
			TIMER_ERR = 0;
		}
	#endif

	// 応答値の初期化
	xbee_result->MODE = 0x00;	// Ver 1.70から自動受信を廃止。そのための追加
	xbee_result->ID = 0x00;
	for( i=0 ; i<8 ; i++ ) xbee_result->FROM[i] = 0x00;
	#ifndef ARDUINO
	#ifndef ARM_MBED
		for( i=0 ; i<2 ; i++ ) xbee_result->SHORT[i] = 0x00;
	#endif
	#endif
	for( i=0 ; i<2 ; i++ ){
		xbee_result->AT[i] = 0xFF;
		xbee_result->GPI.BYTE[i] = 0xFF;
	}
	xbee_result->STATUS = 0xFF;
	for( i=1 ; i<4 ; i++ ) xbee_result->ADCIN[i] = (unsigned int)(0xFFFF);
	xbee_result->ADCIN[0] = 0;
	#ifndef ARDUINO
	#ifndef ARM_MBED
		for( i=0 ; i<6 ; i++ ) xbee_result->ZCL[i] = 0xFF;
	#endif
	#endif
	for( i=0 ; i<CALL_SIZE ; i++ ) xbee_result->DATA[i] = 0x00;

	j = 0;
	#ifdef CACHE_RES
	if( CACHE_COUNTER > 0 ){
		for( i=0 ; i < API_SIZE ; i++) data[i] = CACHE_MEM[0][i];
		/* FIFO 処理 面倒なのでメモリーをコピー */
		for( j=1 ; j < CACHE_COUNTER ; j++)
			for( i=0 ; i < API_SIZE ; i++) CACHE_MEM[j-1][i] = CACHE_MEM[j][i];
		CACHE_COUNTER--;
		return_MODE = 0xFF;
		cache_en = 0xFF;		// キャッシュ有効 フラグ
		#ifdef DEBUG
			lcd_putstr("### CACHE_OUT ### -> ");	// DEBUG
			lcd_disp_hex( CACHE_COUNTER );
			lcd_putch('\n');
		#endif
	}
	#endif
	if( cache_en == 0x00 ) return_MODE = xbee_at_rx( data );	// 受信
	if( return_MODE > 0x00 ){	// キャッシュがある、または受信データがある時
		//	if( xbee_result->MODE == 0x00 ||	// 指定(入力)されたモード値が00(オート)
		//	xbee_result->MODE == data[3] || 	// または、受信結果と一致
		//	cache_en ){ 						// キャッシュ動作の時
		return_MODE = xbee_from_acum( data );
		switch( return_MODE ){
			case MODE_RESP: // 0x97 リモートATコマンドの結果を受信 Remote Command Response
				for(i=0;i<8;i++) xbee_result->FROM[i] = data[5+i];		// アドレスはdata[5]から
				#ifndef ARDUINO
				#ifndef ARM_MBED
					for(i=0;i<2;i++) xbee_result->SHORT[i] = data[13+i];// アドレスはdata[13]から
				#endif
				#endif
			//	for( i=0 ; i<2 ; i++ ) xbee_result->AT[i] = data[15+i]; // ATはdata[15]から
				xbee_result->AT[0] = data[15];
				xbee_result->AT[1] = data[16];
				xbee_result->ID = data[4];								// PACKET_ID
				xbee_result->STATUS = data[17]; 						// statusはdata[17]
				ret = data[17];
				if( xbee_result->STATUS==0x00 ){
					if( xbee_result->AT[0]=='I' && xbee_result->AT[1]=='S' ){	// ATISの受信
						ret=(data[23] & data[20]);	// GPIOの下位を受信マスクで＆して応答(要約応答)
						if( data[19] == 0x00 && data[20] == 0x00 ){
							j = 1;											// デジタルデータが無いとき
						}else{
							j = 0;											// デジタルデータがある時
							xbee_result->GPI.BYTE[1] = (data[22] & data[19]);	// 上位をマスクして応答
							xbee_result->GPI.BYTE[0] = (data[23] & data[20]);	// 下位をマスクして応答
							for( i = 0 ; i<2 ; i++) xbee_result->DATA[i] = xbee_result->GPI.BYTE[i];
						}
						for( i=0; i < 4 ; i++ ){							// この中でjを使用している
							if( (data[21]>>i) & 0x01 ){ 					// data[21] ADCマスク
								xbee_result->ADCIN[i] =  (unsigned int)(data[(2*i+24-2*j)]);
								xbee_result->ADCIN[i] *= (unsigned int)256;
								xbee_result->ADCIN[i] += (unsigned int)(data[2*i+25-2*j]);
								xbee_result->DATA[i*2+2] = data[24+2*i-2*j];
								xbee_result->DATA[i*2+3] = data[25+2*i-2*j];
							}else{
								xbee_result->DATA[i*2+2] = 0xFF;
								xbee_result->DATA[i*2+3] = 0xFF;
								j++;
							}
						}
					}else if( xbee_result->AT[0] == '%' && xbee_result->AT[1] == 'V'){	// BATTの受信
						return_MODE = MODE_BATT;
						xbee_result->GPI.BYTE[1] = data[18];
						xbee_result->GPI.BYTE[0] = data[19];
					//	xbee_result->DATA[1] = data[18];
					//	xbee_result->DATA[0] = data[19];
						xbee_result->ADCIN[0] = xbee_batt_acum(data[18],data[19]);
					}
				}
				break;
			case MODE_GPIN: // 0x92 GPI data を受信するモード (ZigBee IO Data Sample Ex Indicator)
				ret=xbee_gpi_acum( data );
				for(i=0;i<8;i++) xbee_result->FROM[i] = data[4+i];		// アドレスはdata[4]から
				#ifndef ARDUINO
				#ifndef ARM_MBED
					for(i=0;i<2;i++) xbee_result->SHORT[i] = data[12+i];// アドレスはdata[12]から
				#endif
				#endif
				xbee_result->STATUS = data[14]; 						// statusはdata[14] 0x01=Ack 0x02=broadcast
				if( data[16] == 0x00 && data[17] == 0x00 ){
					j = 1;												// デジタルデータが無いとき
				}else{
					j = 0;												// デジタルデータがある時
					xbee_result->GPI.BYTE[1] = (data[16] & data[19]);	// 上位をマスクして応答
					xbee_result->GPI.BYTE[0] = (data[17] & data[20]);	// 下位をマスクして応答
					for( i = 0 ; i<2 ; i++) xbee_result->DATA[i] = xbee_result->GPI.BYTE[i];
				}
				for( i=0; i < 4 ; i++ ){								// この中でjを使用している
					if( (data[18]>>i) & 0x01 ){
						xbee_result->ADCIN[i] =  (unsigned int)(data[2*i+21-2*j]);
						xbee_result->ADCIN[i] *= (unsigned int)256;
						xbee_result->ADCIN[i] += (unsigned int)(data[2*i+22-2*j]);
						xbee_result->DATA[i*2+2] = data[21+2*i-2*j];
						xbee_result->DATA[i*2+3] = data[22+2*i-2*j];
					}else {
						xbee_result->DATA[i*2+2] = 0xFF;
						xbee_result->DATA[i*2+3] = 0xFF;
						j++;
					}
					//printf("ADC[%d]=%d \n",i, (int)(xbee_result->ADC[i]) );
				}
				if( (data[18]>>7) & 0x01 ){
					xbee_result->ADCIN[0] = xbee_batt_acum(data[29-2*j],data[30-2*j]);
				}
			//	xbee_result->DATA[10]=data[2];	// 仮（受信バイト数を確認するため）
				break;
			case MODE_UART: // 0x90 UART data を受信するモード 戻り値はlength
				for(i=0;i<8;i++) xbee_result->FROM[i] = data[4+i];		// アドレスはdata[4]から
				#ifndef ARDUINO
				#ifndef ARM_MBED
					for(i=0;i<2;i++) xbee_result->SHORT[i] = data[12+i];// アドレスはdata[12]から
				#endif
				#endif
				xbee_result->STATUS = data[14]; 						// statusはdata[14]
				if( ( xbee_uart_acum( data )) > 0 ){					// 以上の値は仕様書に記述誤り
					ret = data[2]-0x0C; 			// 12バイト減算
					if( ret >= CALL_SIZE ) ret = CALL_SIZE -1;
					for( i=0; i<ret ; i++ ) xbee_result->DATA[i] = data[15+i];
					// xbee_result->DATA[i] = 0x00;
					xbee_result->STATUS = data[14];
				}
				break;
			#ifndef LITE	// BeeBee Lite by 蘭
			case MODE_UAR2: // 0x91 UART data 2を受信するモード
				for(i=0;i<8;i++) xbee_result->FROM[i] = data[4+i];		// アドレスはdata[4]から
				#ifndef ARDUINO
				#ifndef ARM_MBED
					for(i=0;i<2;i++) xbee_result->SHORT[i] = data[12+i];// アドレスはdata[12]から
					for(i=0;i<6;i++) xbee_result->ZCL[i] = data[14+i];	// ZCL情報data[14]から
				#endif
				#endif
				xbee_result->STATUS = data[20]; 						// statusはdata[20]
				if( ( xbee_uart_acum( data )) > 0 ){
					ret = data[2]-0x12; 			// 18バイト減算
					if( ret >= CALL_SIZE ) ret = CALL_SIZE -1;
					for( i=0; i<ret ; i++ ) xbee_result->DATA[i] = data[21+i];
					// xbee_result->DATA[i] = 0x00;
				}
				break;
			#endif // LITE
			#ifndef LITE	// BeeBee Lite by 蘭
			case MODE_SENS: // 0x94 XB Sensor(1-wire)を受信するモード(未確認)
				for(i=0;i<8;i++) xbee_result->FROM[i] = data[4+i];		// アドレスはdata[4]から
				#ifndef ARDUINO
				#ifndef ARM_MBED
					for(i=0;i<2;i++) xbee_result->SHORT[i] = data[12+i];// アドレスはdata[12]から
				#endif
				#endif
				xbee_result->STATUS = data[14]; 						// statusはdata[14]
				xbee_result->ADCIN[0] = 256*(unsigned int)data[24] + (unsigned int)data[25];
				for( i=1; i < 4 ; i++ ) 				// ADC値を代入、ただしAIN[0]へは代入しない
					xbee_result->ADCIN[i] = 256*(unsigned int)data[2*i+16] + (unsigned int)data[2*i+16];
				for( i=0; i<10 ; i++ ) xbee_result->DATA[i] = data[16+i];
				xbee_result->DATA[10] = data[16+i]; 	// sensor 01:ADC 02:Temp. 60:water present
				break;
			#endif // LITE
			case MODE_IDNT: // 0x95 Node Identifyを受信するモード
				for(i=0;i<8;i++) xbee_result->FROM[i] = data[4+i];		// アドレスはdata[4]から
				#ifndef ARDUINO
				#ifndef ARM_MBED
					for(i=0;i<2;i++) xbee_result->SHORT[i] = data[12+i];	// アドレスはdata[12]から
					for(i=0;i<CALL_SIZE && i<20;i++){
						xbee_result->DATA[i] = data[15+i];
						/*
							 0	15 Source 16-bit address
							 2	17 64-bit Network address
							10	25 NI Strings
							12	27 Parent address
							14	29 Device Type
							15	30 Source Event
							16	31 Profile ID     C1 05
							18	33 Manufacture ID 10 1F
						*/
					}
				#endif
				#endif
				xbee_result->STATUS = data[14]; 						// // statusはdata[14] 0x01=Ack 0x02=broadcast
				ret = data[14];
				switch(data[29]){
					case 0x00:	ret=ZB_TYPE_COORD; break;
					case 0x01:	ret=ZB_TYPE_ROUTER; break;
					case 0x02:	ret=ZB_TYPE_ENDDEV; break;
				}
				break;
			case MODE_RES:	// 0x88 ローカルATコマンドの結果を受信(モードのみ応答)
			//	for( i=0 ; i<2 ; i++ ) xbee_result->AT[i] = data[5+i];	// ATはdata[15]から
				xbee_result->AT[0] = data[5];
				xbee_result->AT[1] = data[6];
				xbee_result->ID = data[4];								// Frame ID
				xbee_result->STATUS = data[7];							// statusはdata[7]
				ret = data[7];
				break;
			case MODE_MODM: // 0x8A Modem Statusを受信
				xbee_result->STATUS = data[4];							// statusはdata[4]
				ret = data[4];
				break;
			case MODE_TXST: // 0x8B UART Transmit Status を受信
				xbee_result->STATUS = data[8];							// delivery statusはdata[8] 0x00=Success
				ret = data[9];											// Discovery status data[9]
			#ifndef ARDUINO
			#ifndef ARM_MBED
			case 0x00:
				break;
			#endif
			#endif
			default:
				#ifdef LCD_H
					xbee_log( 3, "CAUTION:recieved unknown pckt" , xbee_result->MODE );
				#endif
				ret = xbee_result->MODE;
				break;
		}
		xbee_result->MODE = return_MODE;
	}
	#ifdef LCD_H
		// xbee_log( 1, "done:xbee_rx_call" , ret );
	#endif
	return( ret );
}

void xbee_clear_cache(void){
	#ifdef CACHE_RES
		CACHE_COUNTER=0;
	#endif
}

#ifdef EASY_SENSOR
float xbee_sensor(const byte *address, const enum xbee_sensor_type type ){
/*	XBee Sensor(XBee Sensor /L/T または /L/T/H) の測定API (1-wire Sensor Adapterではない)

注意：本コマンドは応答待ちを行うので干渉によるパケット損失があります。
　　　また、センサーがスリープになっている場合はエラーになる場合が多い(タイムアウトする)

	これらのような場合は、xbee_forceで測定指示を出して、xbee_rx_callで受信結果を得ます。
	XBee Sensorはxbee_sensor_resultで受信結果を温度などに変換できます。

	入力：enum xbee_sensor_type {LIGHT,TEMP,HUMIDITY,WATT};    // センサタイプの型
	出力：float センサ読み取り値

設計情報
	リターン信号例
	00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
	7E 00 1B 97 01 00 13 A2 00 -- -- -- -- -- -- 49 53 00 01 08 00 0E 08 00 03 E2 02 62 02 16 A1
	  | len |mo|FI|送信元IEEEアドレス	  |Short|AT(IS)  |01|GPIO |AD|GPIO |ADC1 |ADC2 |ADC3 |CS
	  | 	|de|  | 					  |Adrss|		 |01|MASK |MS|DATA |DATA |DATA |DATA |
	  | 	|  |  | 											  |  |	   |Light|Temp |Humidity
	  | 	|  |						  ADC MASK________________|__|
	  | 	|__|__________________ MODE_RESP(0x97)= ATIS応答
	  |_____|_____________________ Data Length
*/
	XBEE_RESULT xbee_result;
	byte data[API_SIZE];
	byte j;
	float ret = -1.0f;

	xbee_address(address);								// 宛先のアドレスを設定
	if( xbee_tx_rx("RATIS", data , 0) > 0 ){
		if( data[3]==MODE_RESP ){						// d[3] flame ID = 97(RAT応答)
			if( data[19] == 0x00 && data[20] == 0x00 ) j = 0; else j = 2;
			xbee_result.ADCIN[1] = (unsigned int)data[22+j] * 256 + (unsigned int)data[23+j];
			xbee_result.ADCIN[2] = (unsigned int)data[24+j] * 256 + (unsigned int)data[25+j];
			xbee_result.ADCIN[3] = (unsigned int)data[26+j] * 256 + (unsigned int)data[27+j];
			ret = xbee_sensor_result( &xbee_result, type );
		}
	}
	#ifdef LCD_H
		xbee_log( 1, "done:xbee_sensor" , ret );
	#endif
	return( ret );
}
#endif // EASY_SENSOR

byte xbee_init( const byte port ){
/*
	port：PCの場合、オープンするシリアルポート番号
		　Arduinoの場合リトライ回数、0で永久にリトライを繰り返す
	戻り値：0＝エラー　1＝成功までのトライ回数
	Arduino版 portが0で無い時は、portの指定回数リトライしてエラーでもreturnする
*/
	byte i=1;	// iはリセット実行リトライ回数、0で成功するまで永久
	byte j;		// jは色々
	byte k=0;	// kはリセット成功可否フラグ,戻り値
	#ifndef LITE
		#ifndef XBEE_WIFI
			byte address[8];		//={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
		#else // XBEE_WIFI
			byte address[8]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
			// xbee_address(address);
		#endif
	#endif

	#ifdef LCD_H
		xbee_log( 1, "started:xbee_init" , port );
	#endif
	#ifdef H3694
		port_init();					// 汎用入出力ポートの初期化
		timer_sec_init();				// 秒タイマー初期化
		led_green( 1 );
	#endif

	#ifdef H3694
		sci_init( 0 );				// シリアル初期化
		i=port;
	#elif ARDUINO
		sci_init( 0 );				// シリアル初期化
		i=port;
	#else
		#ifdef ARM_MBED
			sci_init( 0 );				// シリアル初期化
			_xbee_time_init();			// Initialize Timer
			i=port;
		#else	// PC
			if( port != 0 ){
				j = sci_init( port );		// シリアル初期化
			}else{
				// ポート検索
				for( i=10 ; i>0; i--){
					j = sci_init( i );
					if( j != 0 ){
						k = xbee_reset();
						if( k > 0 ) i = 1; else j = 0;
					}
				}
			}
			if( j == 0 ){
				fprintf(stderr,"EXIT:Serial Open Error\n");
				exit(-1);
			}
			i=1;
		#endif
	#endif

	#ifdef LCD_H
		#ifndef ARDUINO
			#ifdef H3694
				lcd_enable( 1 );				// 液晶の電源投入
				wait_millisec(15);				// 起動待ち
				lcd_init(); 					// 液晶制御ＩＣのプレ初期化
			#endif
		#endif
		wait_millisec(15);						// 初期化待ち
		lcd_init(); 						// ２回目の初期化（確実な初期化）
		#ifndef ARDUINO
			lcd_control(1,0,0); 			// 液晶制御 表示ON,カーソルOFF,点滅OFF
		#endif
		#ifdef H3694
			lcd_cls();						// 液晶消去
			lcd_goto(LCD_ROW_1);
		#endif
		lcd_putstr( NAME );
		lcd_putstr(" ");
		lcd_putstr( VERSION );
		#ifdef H3694
			lcd_goto(LCD_ROW_2);
		#else
			lcd_putch('\n');
		#endif
		#ifdef LITE
			lcd_putstr(COPYRIGHT);
			lcd_putch('\n');
		#else
			lcd_putstr(COPYRIGHT);
		#endif
		xbee_log( 1, "xbee_reset" , 0 );
	#endif
	#ifdef LITE	// by 蘭
		k = xbee_reset();
		#ifndef H3694
		#ifndef ARDUINO
		#ifndef ARM_MBED	// PC
			if(k==0){
				#ifdef LCD_H
					xbee_log( 5, "EXIT:xbee_init:myaddress" , 0 );
				#endif
				exit(-1);
			}
		#endif
		#endif
		#endif
	#else // Normal mode
		k=1;
		if(i==0) while( !xbee_reset() ) wait_millisec(1000);
		else for(j=0;j<i;j++){
			k=xbee_reset();	// 0だとシリアル異常
			if( k ) break;
			wait_millisec(1000);
		}
		if(k==0){
			#ifndef H3694
			#ifndef ARDUINO
			#ifndef ARM_MBED	// PC
				#ifdef LCD_H
					xbee_log( 5, "EXIT:xbee_init:myaddress" , 0 );
				#endif
				exit(-1);
			#endif
			#endif
			#endif
		}else{	// k>0 すなわち reset成功時  以下、kは使用しないこと（戻り値にする）
			#ifdef LCD_H
				#ifdef H3694
					lcd_cls();
					lcd_goto(LCD_ROW_1);
				#else
					lcd_putstr("\n--------------------\n");
				#endif
			#endif
			#ifdef LCD_H
				xbee_log( 1, "xbee_init:myaddress" , port );
			#endif
			xbee_myaddress( address );	// 自分自身のアドレスを取得
			#ifdef LCD_H
				for(i=0;i<4;i++){
					#ifndef XBEE_WIFI
						lcd_disp_hex(address[4+i]);
					#else // XBEE_WIFI
						/*
						printf("%d",address[i]);
						if(i<3)printf(".");
						*/
						lcd_disp_hex(address[i]);
					#endif
				}
				switch( DEVICE_TYPE ){
					#ifndef XBEE_WIFI
						case ZB_TYPE_COORD: 	lcd_putstr( " COORD."); break;
						case ZB_TYPE_ROUTER:	lcd_putstr( " ROUTER"); break;
						case ZB_TYPE_ENDDEV:	lcd_putstr( " ENDDEV"); break;
					#else
						case XB_TYPE_NULL:		lcd_putstr( " XBee Wi-Fi"); break;
						case XB_TYPE_WIFI10:	lcd_putstr( " XBee Wi-Fi Ver 1.0"); break;
						case XB_TYPE_WIFI20:	lcd_putstr( " XBee Wi-Fi Ver 2.0"); break;
					#endif
					default:	lcd_putstr( " UNKNWON"); break;
				}
				#ifdef H3694
					lcd_goto(LCD_ROW_1);
				#else
					lcd_putstr("\n\n");
				#endif
				xbee_log( 1, "done:xbee_init" , 0 );
			#endif
		}
	#endif
	return(k);
}


byte xbee_end_device(const byte *address, byte sp, byte ir, const byte pin){
/*
XBee子機(エンドデバイス)をスリープモードに設定する
入力：byte *address = 宛先(子機)アドレス
入力：byte sp = 1～28:スリープ間隔（秒）
入力：byte ir = 0:自動送信切、1～65:自動送信間隔（秒）
入力：byte pin = 0:通常のスリープ、 1:SLEEP_RQ端子を有効に設定
出力：戻り値 = 0x00 指示成功、　その他=エラー理由
		1： 	 対応デバイス以外に設定しようとした
		2： 	 スリープ解除しようとしたのに失敗した
		4： 	 スリープ移行待ち時間ST設定の失敗
		8： 	 スリープ間隔SP設定の失敗
		16： ローカル親機へのスリープ時間SP設定の失敗
		32： データ取得間隔IR設定の失敗
		64： スリープ状態設定の失敗
		128：	 スリープオプション設定の失敗
*/
	byte ret=0x00;
	byte data[API_SIZE];
	unsigned int time;

	xbee_address( address );							// 送信先を指定
	if( ir > 65 ) ir = 65;
	time = (unsigned int)ir * 1000; 			// ms単位の数字に変換

	#ifndef XBEE_WIFI // ZigBee
		/* XBee子機から自動送信する周期を設定します */
		data[0] = (byte)(time / 256);
		data[1] = (byte)(time % 256);						// スリープ時間 sleep_time
		if( !xbee_tx_rx("RATIR",data,2) ) ret|=32;

		/* エンドデバイス以外の時に終了する */
		if( xbee_tx_rx("RATVR",data,0) ) if(data[18] != 0x28){
			return( ret );
		}
	#endif

	if( sp == 0x00 ){
		data[0] = 0x00;
		if( !xbee_tx_rx("RATSM",data,1) ) ret|=2;		// スリープ解除
	}else{
		#ifdef XBEE_WIFI
			if(DEVICE_TYPE == XB_TYPE_WIFI20){
				return(1);
			}
			data[0] = 0x01;
			data[1] = 0x40; 					// スリープ時間 sleep_time
			if( !xbee_tx_rx("RATSO",data,2) ) ret|=128;
		#endif

		#ifndef XBEE_WIFI // ZigBee
			/* XBee★親機★にXBee子機のデータ保持期間(28秒)を設定 */
			data[0] = 0x0A;
			data[1] = 0xF0; 					// スリープ時間 sleep_time
			if( !xbee_tx_rx("ATSP",data,2) ) ret|=16;				// 保持時間 sleep_time
		#endif

		/* XBee子機のスリープ移行待ち時間を設定します */
		data[0]=0x01; data[1]=0xF4; 						// 保持時間 500 ms
		if( !xbee_tx_rx("RATST",data,2) ) ret|=4;

		/* XBee子機のスリープ間隔を設定します */
		if( sp > 28 ) sp = 28;
		time = (unsigned int)sp * 100;				// 10ms単位の数字に変換
		data[0] = (byte)(time / 256);
		data[1] = (byte)(time % 256);						// スリープ時間 sleep_time
		if( !xbee_tx_rx("RATSP",data,2) ) ret|=8;

		/* XBee子機をサイクリックスリープに設定します */
		if( pin ) data[0]=0x05; else data[0]=0x04;	// 0x00:常に動作 0x04:低消費電力動作
		if( !xbee_tx_rx("RATSM",data,1) ) ret|=64;	// 0x05:SLEEP_RQ有効
	}

	#ifdef LCD_H
		xbee_log( 1, "done:end_device" , ret );
	#endif
	return( ret );
}

#ifndef LITE
byte _xbee_i2c(byte dio,byte level,byte *data){
/*	dio 	= 11:SCL	12:SDA
	level	= 0:L 1:Open
	data	ATコマンド応答用データ

	data[0]=0x03;	xbee_at_tx("SATP1",data,1); 	// Port 11(SCL) H Imp
	data[0]=0x04;	xbee_at_tx("SATP1",data,1); 	// Port 11(SCL) L Out
	data[0]=0x03;	xbee_at_tx("SATP2",data,1); 	// Port 12(SDA) H Imp
	data[0]=0x04;	xbee_at_tx("SATP2",data,1); 	// Port 12(SDA) L Out
*/
	char at[6]="SATPX";
	if(dio>=10) dio = dio%10;
	if(dio>=2) dio=2; else dio=1;
	at[4]='0'+dio;
	data[0]=level;
	if(level>=10) data[0]=level%10;
	if(level==0) data[0]=4; // Out Low
	if(level==1) data[0]=3; // High Imp
	return( xbee_at_tx(at,data,1) );	// 戻り値：0ならエラー発生
}

byte _xbee_i2c_start(byte *data){
	byte ret=1; // 戻り値：0ならエラー発生
	ret *= _xbee_i2c(1/*SCL*/,1,data);					// Port 11(SCL) H Imp
	ret *= xbee_tx_rx("RATAC",data,0);					// クロックHレベル固定
	ret *= _xbee_i2c(2/*SDA*/,1,data);					// Port 12(SDA) H Imp
	ret *= _xbee_i2c(2/*SDA*/,0,data);					// Port 12(SDA) L Out
	ret *= _xbee_i2c(1/*SCL*/,0,data);					// Port 11(SCL) L Out
	if(ret==0) xbee_log( 5, "ERROR:Failed I2C TX Output" , ret );
	return(ret);
}

byte _xbee_i2c_tx(const byte in, byte *data){
	byte i;
	for(i=0;i<8;i++){
		if( (in>>(7-i))&0x01 ){
				_xbee_i2c(2/*SDA*/,1,data); 		// Port 12(SDA) H Imp
		}else	_xbee_i2c(2/*SDA*/,0,data); 		// Port 12(SDA) L Out
		/*Clock*/
		_xbee_i2c(1/*SCL*/,1,data); 				// Port 11(SCL) H Imp
		_xbee_i2c(1/*SCL*/,0,data); 				// Port 11(SCL) L Out
	}
	/* ACK受信待ち(ACKはスレーブから) */
	_xbee_i2c(1/*SCL*/,1,data); 					// Port 11(SCL) H Imp
	_xbee_i2c(2/*SDA*/,1,data); 					// Port 12(SDA) H Imp　逆
//	xbee_tx_rx("RATAC",data,0); // 本来は不要だがEnd Deviceの時にRATISを確実に受信するために入れてみた
	i=xbee_tx_rx("RATIS",data,0);
	if( i==0) xbee_log( 5, "ERROR:I2C no RAT RESP" , i );
	if( (data[22]>>4)&0x01 ){
		xbee_log( 5, "ERROR:I2C no ACK" , data[22] );
		i=0;
	}
//	printf("ACK=%d ATIS=%d\n",(data[22]>>4)&0x01,i);
	return(i);
}

byte xbee_i2c_init(const byte *address){
/*
XBee I2Cインタフェースとして使用するための初期設定
入力：byte *address　リモートXBee子機のIEEEアドレス
*/
	byte data[API_SIZE];
	byte i;
	for(i=3;i>0;i--){	// リトライ3回まで
		xbee_address(address);
		_xbee_i2c(1/*SCL*/,1,data); 				// Port 11(SCL) H Imp
		_xbee_i2c(2/*SDA*/,1,data); 				// Port 12(SDA) H Imp
		if( xbee_tx_rx("RATIS",data,0) ){
			if( ((data[22]>>3)&0x03) == 0 ) break;
		}
	}
	delay(200);
	return(i);
}

byte xbee_i2c_read(const byte *address, byte adr, byte *rx, byte len){
/*
子機XBeeデバイス(リモート先)のI2Cから指定バイトの情報を読み取る関数
入力：byte *address = リモートXBee子機のIEEEアドレス
入力：byte adr = I2Cアドレス ／SHT 温度測定時0x01	 湿度測定時0x02
出力：byte *rx = 受信データ用ポインタ
入力：byte len = 受信長、０の時は Sensirion SHT用２バイト受信
			port=11 	DIO11	XBee_pin  7 	SCL
			port=12 	DIO12	XBee_pin  4 	SDA
・I2Cマスタ機能のみを実装（スレーブとしては動作しない）
・７ビットアドレスのみ対応
・１バイトの受信に５秒以上かかる（かなり遅いし、多量のXBee通信が発生する）
*/
	byte data[API_SIZE];
	byte ret,i,sht=0;

	xbee_address(address);							// 宛先のアドレスを設定
	if(len){	// stop シーケンス ＋ start シーケンス
		_xbee_i2c_start(data);
	}else{		// Sensirion SHT用	 TS シーケンス
		_xbee_i2c(1/*SCL*/,0,data); 				// Port 11(SCL) L Out
		_xbee_i2c(1/*SCL*/,1,data); 				// Port 11(SCL) H Imp
		_xbee_i2c(2/*SDA*/,0,data); 				// Port 12(SDA) L Out
		_xbee_i2c(1/*SCL*/,0,data); 				// Port 11(SCL) L Out
		_xbee_i2c(1/*SCL*/,1,data); 				// Port 11(SCL) H Imp
		_xbee_i2c(2/*SDA*/,1,data); 				// Port 12(SDA) H Imp
		_xbee_i2c(1/*SCL*/,0,data); 				// Port 11(SCL) L Out
		_xbee_i2c(2/*SDA*/,0,data); 				// Port 12(SDA) L Out
		sht=1;	len=2;	// SHTモード
		xbee_tx_rx("RATAC",data,0);
		adr = adr<<1;
	}
	adr |= 0x01;	// RW=1 受信モード
	if( _xbee_i2c_tx(adr, data)==0 ) return(0); // アドレス設定
	if( !sht ){ // SHTセンサで無い時
		_xbee_i2c(2/*SDA*/,0,data); 				// Port 12(SDA) L Out
		_xbee_i2c(1/*SCL*/,0,data); 				// Port 11(SCL) L Out
	}

	/* スレーブ待機状態待ち */
	if(!sht){	// I2C
		/*
		for(i=0;i<4;i++){
			ret=xbee_tx_rx("RATIS",data,0);
			if( (((data[22]>>4)&0x01) == 1) &&		// ACKが解除された	  //data[22]b4=Port 12(SDA)
				(((data[22]>>3)&0x01) == 1) ) break;// スレーブがCLKホールドしていない // data[22]b3=Port 11(SCL)
			wait_millisec(500);
		}
				// printf("i=%d\n",i);
		if(i==4){
			xbee_log( 5, "ERROR:I2C Clock Holded" , data[22] );
			return(0);
		}
		*/
	}else{		// SHT
		_xbee_i2c(1/*SCL*/,0,data); 				// Port 11(SCL) L Out
		for(i=0;i<4;i++){
			ret=xbee_tx_rx("RATIS",data,0);
			if( ((data[22]>>4)&0x01) == 0 ) break;// 測定完了	//data[22]b4=Port 12(SDA)
		}	// printf("Slave Holds SDA i=%d\n",i);
		if(i==4){
			xbee_log( 5, "ERROR:I2C SDA Holded" , data[22] );
			return(0);
		}
	}
	_xbee_i2c(1/*SCL*/,0,data); 						// Port 11(SCL) L Out

	/* 受信データ */
	for(ret=0;ret<len;ret++){
		if(ret){	// 初回以外の時はACKを応答する
			_xbee_i2c(2/*SDA*/,0,data); 				// Port 12(SDA) L Out
			_xbee_i2c(1/*SCL*/,1,data); 				// Port 11(SCL) H Imp
			_xbee_i2c(1/*SCL*/,0,data); 				// Port 11(SCL) L Out
			_xbee_i2c(2/*SDA*/,1,data); 				// Port 12(SDA) H Imp
		}
		rx[ret]=0x00;
		for(i=0;i<8;i++){
			_xbee_i2c(1/*SCL*/,1,data); 				// Port 11(SCL) H Imp
		//	xbee_tx_rx("RATAC",data,0);
			if( xbee_tx_rx("RATIS",data,0)==0x00) break;	// error
			rx[ret] |= ((data[22]>>4)&0x01)<<(7-i); 	//data[22] b4=Port 12(SDA)
			_xbee_i2c(1/*SCL*/,0,data); 				// Port 11(SCL) L Out
		}
		if(i!=8) break; // error
	}
	_xbee_i2c(1/*SCL*/,1,data); 						// Port 11(SCL) H Imp
	if(sht) _xbee_i2c(1/*SCL*/,0,data); 				// Port 11(SCL) L Out
	xbee_tx_rx("RATAC",data,0);
	return(ret);
}

byte xbee_i2c_write(const byte *address, byte adr, byte *tx, byte len){
/*
子機XBeeデバイス(リモート先)のI2Cから指定バイトの情報を書き込む関数
入力：byte *address = リモートXBee子機のIEEEアドレス
入力：byte adr = I2Cアドレス ／SHT 温度測定時0x01	 湿度測定時0x02
入力：byte *tx = 送信データ用ポインタ
入力：byte len = 送信データ長、０の時は Sensirion SHT用２バイト受信
			port=11 	DIO11	XBee_pin  7 	SCL
			port=12 	DIO12	XBee_pin  4 	SDA
*/
	byte data[API_SIZE];
	byte ret;

	xbee_address(address);							// 宛先のアドレスを設定
	if(len) _xbee_i2c_start(data);
	else return(0);
	adr &= 0xFE;	// RW=0 送信モード
	if( _xbee_i2c_tx(adr, data)==0 ) return(0); // アドレス設定
	/* スレーブ待機状態待ち */
	/*
	for(i=0;i<4;i++){
		ret=xbee_tx_rx("RATIS",data,0);
		if( (((data[22]>>4)&0x01) == 1) &&		// ACKが解除された	  //data[22]b4=Port 12(SDA)
			(((data[22]>>3)&0x01) == 1) ) break;// スレーブがCLKホールドしていない // data[22]b3=Port 11(SCL)
		wait_millisec(500);
	}
			// printf("i=%d\n",i);
	if(i==4){
		xbee_log( 5, "ERROR:I2C Clock Holded" , data[22] );
		return(0);
	}
	*/
	_xbee_i2c(2/*SDA*/,0,data); 				// Port 12(SDA) L Out
	_xbee_i2c(1/*SCL*/,0,data); 				// Port 11(SCL) L Out

	/* 送信データ */
	for(ret=0;ret<len;ret++){
		_xbee_i2c_tx(tx[ret],data);
		_xbee_i2c(2/*SDA*/,0,data); 				// Port 12(SDA) L Out
		_xbee_i2c(1/*SCL*/,0,data); 				// Port 11(SCL) L Out
	}
	return(ret);
}
#endif // LITE

byte xbee_delay(unsigned int ms){
/*
待ち時間。受信値はバッファへ保存します。
入力：unsigned int(shortを想定) ms	= 時間[ms]
戻り値：受信パケット数
*/
	#ifdef LITE // BeeBee Lite by 蘭
		wait_millisec( ms );
		return 0;
	#else
		byte i,ret=0;
		byte data[API_SIZE];

		while(ms>0){
			#ifdef CACHE_RES
				if( CACHE_COUNTER < CACHE_RES ){
					if( xbee_at_rx( data ) > 0){
						for( i=0 ; i < API_SIZE ; i++) CACHE_MEM[CACHE_COUNTER][i] = data[i];
						CACHE_COUNTER++;
						ret++;
						#ifdef DEBUG
							lcd_putstr("### CACHE_RES ### <- ");	// DEBUG
							lcd_disp_hex( CACHE_COUNTER );
							lcd_putch('\n');
						#endif
					}
				}
			#endif
			if( ms > 100 ){
				wait_millisec( 100 );
				ms -= 100;
			}else{
				wait_millisec( ms );
				ms = 0;
			}
		}
		return(ret);
	#endif
}
