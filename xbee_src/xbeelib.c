//xbee用ライブラリ xbee_lib.c

#include <stdio.h>                                  // 標準入出力用
#include <stdlib.h>                                 // system関数用
#include <fcntl.h>                                  // シリアル通信用(Fd制御)
#include <termios.h>                                // シリアル通信用(端末IF)
#include <string.h>                                 // strncmp,bzero用
#include <sys/time.h>                               // fd_set,select用
#include <unistd.h>                                 // read,usleep用
#include <ctype.h>                                  // isxdigit用
#include <time.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "xbeelib.h"

//define------------------------------------------------------------------
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
#define 	API_SIZE	128 		// 受信用APIデータ長(32～255)
#define API_TXSIZE	64				// 送信用APIデータ長(32～255) シリアル送信最大長=API_TXSIZE-18バイト

#define TIMER_SEC	time1s256() 	// TIMER_SECのカウントアップの代わり

typedef unsigned char byte;

//グローバル変数宣言(static)----------------------------------------------
static int ComFd;                                   // シリアル用ファイルディスクリプタ
static struct termios ComTio_Bk;                    // 現シリアル端末設定保持用の構造体
static byte ADR_DEST[]= 	{0x00,0x13,0xA2,0x00,0x00,0x00,0x00,0x00};	//宛先のIEEEアドレス(変更可)
//ショートアドレス／本ライブラリでの宛先指定はIEEEのみを使う
static byte PACKET_ID = 0;							//送信パケット番号
static byte SADR_DEST[]=	{0xFF,0xFE};			//ブロード(ショート)アドレス
/* XBeeのデバイスタイプ ATVRの上２ケタ */
static byte DEVICE_TYPE = ZB_TYPE_COORD;			// Coord=0x21 Router=23 ED=29 Wi-Fi=10
/* IEEEアドレス(最小限の通信対象をライブラリ側で保持する)／複数のデバイスへの対応はアプリ側で行う*/
static byte ADR_FROM[]= 	{0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF};	//差出人のIEEEアドレス(変更可)
static byte xbee_com_port;

//関数プロトタイプ宣言---------------------------------------------------
void wait_millisec(unsigned int ms);
byte time1s256();

//xbee_init関連関数群
//byte xbee_init(const byte port);
//以下sci_init関数群
byte sci_init(byte port);
int open_serial_port(char *modem_dev);
char read_serial_port(void);
// int gets_serial_port(char *data,int len);
int write_serial_port(char *data,byte len);
int puts_serial_port(char *data);
int close_serial_port(void);
//以下xbee_reset関数群
byte xbee_reset( void );
byte sci_write_check(void);
void sci_clear(void);
//以下xbee_myadress関数群
byte xbee_myaddress( byte *address );
byte xbee_tx_rx(const char *at, byte *data, byte len);
byte xbee_at_tx(const char *at, const byte *value, const byte value_len);
byte xbee_at_rx(byte *data);

//以上がxbee_init関連関数群

//xbee_atnj関連関数群
//byte xbee_atnj(const byte timeout);
byte xbee_from_acum( const byte *data );

//xbee_from 関連関数群
//void xbee_from(byte *address);

//xbee_ratnj関連関数群
//byte xbee_ratnj(const byte *address, const byte timeout);
void xbee_address(const byte *address);

//xbee_uart関連関数群
//byte xbee_uart(const byte *address, const char *in);
byte xbee_putch( const char c );
byte xbee_putstr( const char *s ); //文字列を送る

void xbee_disp_1( int x );
void xbee_disp_2( int x );
void xbee_disp_3( int x );
void xbee_disp_5( int x );
void xbee_putint(int x); //int型のデータを送る
void xbee_putdouble(double x); //double型のデータを送る

enum xbee_sensor_type{ LIGHT,TEMP,HUMIDITY,WATT,BATT,PRESS,VALUE,TIMES,NA };	// センサタイプの型
enum xbee_port_type{ DISABLE=0, VENDER=1, AIN=2, DIN=3, DOUT_L=4, DOUT_H=5 };
															// GPIOの設定の型

//関数本体---------------------------------------------------------------
void wait_millisec(unsigned int ms){
	usleep(1000*ms);
}

byte time1s256(){
			time_t sec;
			time( &sec );
			return( (byte)sec );
}

//xbeeの初期化を行う
byte xbee_init( const byte port ){
	byte i=1;	// iはリセット実行リトライ回数、0で成功するまで永久 //4959
	byte j;		// jは色々
	byte k=0;	// kはリセット成功可否フラグ,戻り値
	byte address[8];		//={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	fprintf(stderr,"start initializer\n");
	if( port != 0 ){   //4992
		j = sci_init( port );		// シリアル初期化
		fprintf(stderr,"finished sci_init\n");
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
		k=1;
		if(i==0) while( !xbee_reset() ) wait_millisec(1000);
		else for(j=0;j<i;j++){
			k=xbee_reset();	// 0だとシリアル異常
			if( k ) break;
			wait_millisec(1000);
		}
		if(k==0){
				exit(-1);
				fprintf(stderr,"exit\n");
		}else{	// k>0 すなわち reset成功時  以下、kは使用しないこと（戻り値にする）
			xbee_myaddress( address );	// 自分自身のアドレスを取得
		}
	return(k);
}

//以下sci_init関連関数群
typedef unsigned char byte;
static int ComFd;                                   // シリアル用ファイルディスクリプタ
static struct termios ComTio_Bk;                    // 現シリアル端末設定保持用の構造体

int open_serial_port(char *modem_dev){
    struct termios ComTio;                          // シリアル端末設定用の構造体変数
    speed_t speed = B9600;                          // 通信速度の設定 9600 bps
    ComFd=open(modem_dev, O_RDWR|O_NONBLOCK);       // シリアルポートのオープン
    if(ComFd >= 0){                                 // オープン成功時
        printf("com=%s\n",modem_dev);               // 成功したシリアルポートを表示
        tcgetattr(ComFd, &ComTio_Bk);               // 現在のシリアル端末設定状態を保存
        ComTio.c_iflag = 0;                         // シリアル入力設定の初期化
        ComTio.c_oflag = 0;                         // シリアル出力設定の初期化
        ComTio.c_cflag = CLOCAL|CREAD|CS8;          // シリアル制御設定の初期化
        ComTio.c_lflag = 0;                         // シリアルローカル設定の初期化
        bzero(ComTio.c_cc,sizeof(ComTio.c_cc));     // シリアル特殊文字設定の初期化
        cfsetispeed(&ComTio, speed);                // シリアル入力の通信速度の設定
        cfsetospeed(&ComTio, speed);                // シリアル出力の通信速度の設定
        ComTio.c_cc[VMIN] = 0;                      // リード待ち容量0バイト(待たない)
        ComTio.c_cc[VTIME] = 0;                     // リード待ち時間0.0秒(待たない)
        tcflush(ComFd,TCIFLUSH);                    // バッファのクリア
        tcsetattr(ComFd, TCSANOW, &ComTio);         // シリアル端末に設定
    }
    return ComFd;
}

char read_serial_port(void){
    char c='\0';                                    // シリアル受信した文字の代入用
    fd_set ComReadFds;                              // select命令用構造体ComReadFdを定義
    struct timeval tv;                              // タイムアウト値の保持用
    FD_ZERO(&ComReadFds);                           // ComReadFdの初期化
    FD_SET(ComFd, &ComReadFds);                     // ファイルディスクリプタを設定
    tv.tv_sec=0; tv.tv_usec=100000;                 // 受信のタイムアウト設定(100ms)
    if(select(ComFd+1, &ComReadFds, 0, 0, &tv)) read(ComFd, &c, 1); // データを受信
    return c;                                       // 戻り値＝受信データ(文字変数c)
}

int write_serial_port( char *data, byte len ){
    int i;
    for(i=0;i<len;i++){
        if(write(ComFd,&data[i],1) != 1){
            fprintf(stderr,"write_serial ERR:d[%02d]=0x%02x\n",i,(byte)data[i]);
            return -1;
        }
    }
    return i;
}

int puts_serial_port( char *data ){
    return write_serial_port(data,strlen(data));
}

int close_serial_port(void){
    tcsetattr(ComFd, TCSANOW, &ComTio_Bk);
    return close(ComFd);
}

byte sci_init( byte port ){
    char modem_dev[13] = "/dev/ttyS00";
    char alter_dev[13] = "/dev/ttyUSB0";
    byte xbee_com_port;

    if( port <= 10){
        modem_dev[9] = (char)( port - 1 + (byte)'0' );
        modem_dev[10]= '\0';
    }else if( port < 64 ){                                  // COM64は使用不可
        snprintf(&modem_dev[9], 3, "%02d", port - 1);
    }else if( port < 74 ){
        snprintf(&modem_dev[8], 2, "%1d", port - 64);       // tty0～9
    }else if( port < 128 ){
        snprintf(&modem_dev[8], 3, "%02d", port - 64);      // tty10～63
    }else if( (port&0xF0) == 0xA0 ){
        if( port > 0xA9 ) port = 0xAF - (port&0xF);
        snprintf(&modem_dev[8], 5, "AMA%1X", port&0x0F);    // ttyAMA0～9
    }else if( (port&0xF0) == 0xB0 ){
        snprintf(&modem_dev[8], 5, "USB%1X", port&0x0F);    // ttyUSB0～9
    }else{
        fprintf(stderr,"ERR:sci_init port=%d\n",port);
        return(0);
    }
    if( open_serial_port( modem_dev )<0 ){    // 失敗時(alterに変更する)
        usleep( 100 );
        close_serial_port();    // open出来ていないが念のために閉じる
        usleep( 100 );
        if( port <= 9 ){
            snprintf(&alter_dev[8], 5, "USB%1X", port&0x0F);    // ttyUSB0～9
            if( open_serial_port( alter_dev )>=0 ){
                strcpy(modem_dev,alter_dev);
                port += 0xB0;
                xbee_com_port = port;
            }else xbee_com_port = 0;
        }else xbee_com_port = 0;
    }else xbee_com_port = port;
    if( xbee_com_port ) fprintf(stderr,"Serial port = ");
    else fprintf(stderr,"FAILED serial ");
    if( port < 64){
        fprintf(stderr,"COM%d",port);
    }else if( port < 73 ){
        fprintf(stderr,"%1d", port - 64);       // tty0～9
    }else if( port < 128 ){
        fprintf(stderr,"%02d", port - 64);      // tty10～63
    }else if( (port&0xF0) == 0xA0 ){
        fprintf(stderr,"AMA%1X", port&0x0F);    // ttyAMA0～9
    }else if( (port&0xF0) == 0xB0 ){
        fprintf(stderr,"USB%1X", port&0x0F);    // ttyUSB0～9
    }
    fprintf(stderr," (%s,0x%02X)\n",modem_dev,port);

    return( xbee_com_port );
}

//以下xbee_reset関数群
byte xbee_reset( void ){
/*	XBeeモジュールのリセット
	戻り値：0＝エラー、強制終了あり(PC版)、無限定しあり(H8版)
*/
	byte ret=0;	// 戻り値 0:異常終了
	byte value[API_SIZE];
	value[0] = 0x00;
	sci_write_check();
	sci_clear();						// シリアル異常をクリア
	DEVICE_TYPE = 0x20; 				// タイプ名を初期化
	wait_millisec(100);
			int i=0;
      for(i=0;i<4;i++){
        ret=xbee_tx_rx("ATVR",value,0);
				if( ret > 0){
					DEVICE_TYPE = value[8];
					if( DEVICE_TYPE != ZB_TYPE_COORD &&
						DEVICE_TYPE != ZB_TYPE_ROUTER &&
						DEVICE_TYPE != ZB_TYPE_ENDDEV){ // VRの確認
							fprintf( stderr,"EXIT:XBEE NOT IN API MODE" );
							exit(-1);
					}
					else break;
				}
				else{
					if(i==3){
					//3回トライしてもret=0
					fprintf( stderr,"EXIT:NO RESPONCE FROM XBEE" );
					exit(-1);
					}
				}
			}
			wait_millisec(1000);
			wait_millisec(100);
			ret = xbee_tx_rx( "ATFR", value ,0 );	//ATFR 初期化
			if( ret == 0){
					fprintf( stderr,"EXIT:CANNOT RESET XBEE" );
					exit(-1);
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
	return( ret );
}

byte sci_write_check(void){ //送信された出力が実際に端末に送信されるまで待機
			tcdrain( ComFd );
			return( 1 );
}

void sci_clear(void){
				tcflush(ComFd,TCIFLUSH);
}

//以下アドレス取得関数群
byte xbee_myaddress( byte *address ){
/*
自分自身のIEEEアドレスを取得する ／ XBee Wifi(PC)の場合は設定する
	byte *address : IEEEアドレスを代入する
	戻り値＝１で正常読み込み、２は書き込み。０は異常
*/
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
		return( ret );
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

	byte err,retry;		//errの値が0であれば、成功
	byte r_dat = 10;	// AT=0、RAT=10、TX=未定	リモートATと通常ATの応答値dataの代入位置の差
	byte r_at = 1;		// AT=0、RAT=1				 リモートの可否
	unsigned int wait_add = 0;

		if( at[0] == 'A' && at[1] == 'T' ){
			r_dat = 0; r_at=0;
			if( at[2] == 'W' && at[3] == 'R' ) wait_add = 100;			// ATWR 110～1100ms
		} else if ( at[0] == 'R' && at[1] == 'A' && at[2] == 'T' ){
			r_dat = 10; r_at=1;
			if( at[3] == 'W' && at[4] == 'R' ) wait_add = 100;	// RATWR 120～1200ms
		}
		if( xbee_at_tx( at ,data ,len ) > 0){  //送信は成功
			err = 12;							// 受信なしエラー 受信すればこれが0になる
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
						if( data[7+r_dat] == 0x00 ){
							err=0;	//受信成功
						}else err = data[7+r_dat];	// ATのERRORコード (AT:data[7] RAT:data[17])
					}
					else{				// 受信したパケットIDまたはATコマンドが相違しているとき
					}
				}
			}
		}else				err = 11;				// そもそも送信失敗
		wait_millisec(1);	// 直前のコマンド応答がすぐに返った時にキャッシュに貯めれないことを防止する

	if( err ){
	wait_millisec( 1000 );		// 応答待ち状態で、次々にコマンドを送るとXBeeモジュールが非応答になる対策
  }
	return( !err );
}


/* (ドライバ)ATコマンド送信 */
byte xbee_at_tx(const char *at, const byte *value, const byte value_len){
/*
	処理：リモートＡＴコマンドの送信を行うXBeeドライバ部
	入力：ATコマンドat[] ＝ "AT**"はローカルATで、"RAT***"がリモートＡＴ
	　　　　　　　　　　　　TXがデータ送信モード
	　　　データvalue[]　＝ 各ＡＴコマンドで引き渡すデータ値
	　　　value_len　　　＝ その長さ
	戻り値：送信したAPIサービス長。送信しなかった場合は0
	static byte PACKET_ID = 0;							//送信パケット番号
	static byte ADR_DEST[]= 	{0x00,0x13,0xA2,0x00,0x00,0x00,0x00,0x00};	//宛先のIEEEアドレス(変更可)
	ショートアドレス／本ライブラリでの宛先指定はIEEEのみを使う
	static byte SADR_DEST[]=	{0xFF,0xFE};			//ブロード(ショート)アドレス

	APIフレーム
	1バイト目(data_api[0])=0x7E
	2バイト目(data_api[1])= length上位
	3バイト目(data_api[2])= length下位
	4バイト目(data_api[3])=通信モード ATコマンド0x08 Remoteコマンド 0x17
	以下データ
	最後にチェックサム
*/

	char data_api[API_TXSIZE];
	byte i;
	byte len;				// APIサービス長
	byte check=0xFF;		// チェックサム
	byte data_position=5;	// 送信データdata_api[]の何処にvalue[]を入れるか
	byte ret=0;

	if( PACKET_ID == 0xFF ){
		PACKET_ID=0x01;
	}else{
		PACKET_ID++;  //デフォルトは0なのでPacket_ID = 0x01になる
	}
	len=0;
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
			default:
				break;
		}
		if( len ){
			data_api[0]=(char)0x7E; 			// デリミタ
			data_api[1]=(char)0x00; 			// パケット長の上位(送らない前提(パケット長は短いと仮定))
			for( i=3 ; i < data_position ; i++) check -= (byte)data_api[i];
			if( value_len > 0 ){
				for( i=0 ; i<value_len; i++){
					data_api[data_position + i] = (char)value[i];
					check -= (byte)data_api[data_position + i];
					len++;
				}
			}
			data_api[2]    =(char)len;
			data_api[len+3]=(char)check;    //チェックサムをフレームの最後に挿入
				check = sci_write_check();		// 以降 checkはシリアルバッファ確認に使用する
				/*シリアルデータ送信 */
				if(  check > 0 ){
					if( write_serial_port( data_api, (byte)(len+4) ) == 0 ){
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
							write_serial_port( data_api, (byte)(len+4) );	// 再送信
					}
					ret=len+3;
				}else{
					ret=0;
				}
		}
	return( ret );
}

byte xbee_at_rx(byte *data){
/*
	リモートＡＴコマンドの応答を受信するXBeeドライバ部
	処理：XBeeからの受信データをdata[]へ代入(応答)する
	入出力：APIデータdata[] 前３バイトは'7E'+len(2) data[0]が0x00の場合はタイムアウト
	リターン：APIサービス長、0はタイムアウト
*/
	byte i;
	byte len;			// APIサービス長－３ (APIフレームの前３バイトとチェックサム１バイトを除く)
	unsigned int leni;	// 実際のAPIサービス長と使用するAPIサービス長の差(データ破棄用)
	byte check = 0xFF;	// チェックサム
	byte ret=0;

	/* 受信処理 */
		data[0] = read_serial_port();				// 1ms待ち受けで受信
		if( data[0] == 0x7E ) { 				// 期待デリミタ0x7E時
			for( i=1;i<=2;i++ ){
					data[i] = read_serial_port();
			}
			if(data[1] == 0x00) len = data[2];
			else len = 0xFF - 4;		// API長が255バイトまでの制約(本来は64KB)
			if( len > (API_SIZE-4) ) len = API_SIZE-4;
			leni = (unsigned int)data[1] * 256 + (unsigned int)data[2] - (unsigned int)len;
				// 通常は0。lenが本来の容量よりも少ない場合に不足分が代入されれる
			for( i=0 ; i <= len ; i++){ // i = lenはチェックサムを入力する
				data[i+3] = read_serial_port();
				if( i != len) check -= data[i+3];	// チェックサムのカウント
			}
			while( leni > 0 ){
				data[len+3] = read_serial_port();	// データの空読み(lenは固定)
				if( leni != 1 ) {
					check -= data[len+3];		// leni=0の時はCheck sumなので減算しない
				}
				leni--;
			}
			if( check == data[len+3] ) ret = len +3;
			else ret = 0;
		}
	return( ret );
}


//xbee_init関連関数群はここまで

//xbee_atnj関連関数群
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
		byte data[API_SIZE];
		byte i=0, ret, timer;

		data[0]=timeout;
		ret = xbee_tx_rx( "ATNJ", data ,1 );
		if( ret > 0 ){
			wait_millisec(100);
			if( timeout == 0xFF ){
				/*常に参加待ち受けするテスト専用(非実用)*/
				ret=0xFF;
				wait_millisec(1000);			// NJ値の書込み時間待ち
			}else{  //3980
				if( timeout > 0x09 ){
					/*参加待ち受けの開始*/
					timer = TIMER_SEC + timeout + 0x01 ;
					ret = 0x00;
					while( timer != TIMER_SEC && ret != MODE_IDNT){ // MODE_IDNT(0x95)はネットワーク参加
						if( xbee_at_rx( data ) > 0x00 ){	// データの受信(戻り値は受信種別mode値)
							ret = xbee_from_acum(data);
							if( ret == MODE_IDNT ){ 	// 受信モードがMODE_IDNT(0x95) Node Identifyの場合
								for(i=0;i<8;i++) ADR_DEST[i]=ADR_FROM[i];			// 宛先を発見デバイスに設定
								sci_clear();
								}else ret=0x00;
						}
					}
				}
				/*ジョイン拒否設定*/
				data[0]=0x00;
				xbee_tx_rx( "ATNJ", data ,1 );
			}
		}else{
		}
		return( ret );
}

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
		ret = 0;
	}
	return( ret );
}

//xbee_from関数群
void xbee_from(byte *address){
/*
	最後に受信したデバイスの送信元アドレスの読み込み。
	但し、xbee_rx_callで受信した場合の送信元アドレスは
	「構造体XBEE_RESULT変数.FROM」で読むこと。
	（キャッシュ値での受信の場合があるので）
	出力：byte *address IEEEアドレス
*/
	byte i;
		for(i=0; i< 8 ;i++ ) address[i] = ADR_FROM[i];
}

//xbee_ratnj関連関数群
byte xbee_ratnj(const byte *address, const byte timeout){
/*
	前記xbee_atnjをリモート先(子機)に対して設定する関数
	入力：byte *address = 宛先(子機)アドレス
	入力：timeout = 0でジョイン拒否、1～254で待ち秒数、255で常時ジョイン許可
	出力：byte 戻り値 = 0x00失敗、0xFF成功

注意：本コマンドは応答待ちを行うので干渉によるパケット損失があります。
*/
		byte data[API_SIZE];
		byte ret=0x00;

		xbee_address(address);							// 宛先のアドレスを設定
		data[0]=timeout;
		if( xbee_tx_rx( "RATNJ", data ,1 ) > 0 ){
			ret=0xFF;
		}
		return( ret );
}

void xbee_address(const byte *address){
/*
送信用の宛先アドレス設定用の関数
	入力：byte *address = 宛先(子機)アドレス
*/
	byte i;

		for(i=0; i< 8 ;i++ ) ADR_DEST[i] = address[i];
			SADR_DEST[0] = 0xFF;
			SADR_DEST[1] = 0xFE;
}

//以下xbee_uart関連関数群
byte xbee_uart_char(const byte *address, const char *in){
/*
	入力：byte *address = 宛先(子機)アドレス
	　　　char *in = 送信するテキスト文字。最大文字数はAPI_TXSIZE-1
	出力：戻り値 = 送信パケット番号PACKET_ID。0x00は失敗。
*/
	byte ret=0;
	xbee_address( address );						// 宛先のアドレスを設定
	if( xbee_putstr(in) > 0 ) ret = PACKET_ID;
	return( ret );
}

void xbee_uart_int(const byte *address,int in){
/*
	入力:byte *adress = 宛先(子機)アドレス
	    int in = 送信するint型の数字。
	出力:戻り値 = 送信パケット番号PACKET_ID,0x00は失敗
*/
	byte ret=0;
	xbee_address( address );						// 宛先のアドレスを設定
	xbee_putint(in);
}

void xbee_uart_double(const byte *address,double in){
	//double型を有効数字4桁で送信
	byte ret=0;
	xbee_address( address );						// 宛先のアドレスを設定
	xbee_putdouble(in);
}

byte xbee_putch( const char c ){
	byte data[2];
	byte len;
	data[0] = (byte)c;
	if( xbee_at_tx( "TX", data , 1) == 0 ) len=0; else len=1;
	return( len );
}

byte xbee_putstr( const char *s ){
/*
	文字を送信する
	入力：char *s
	出力：送信データ長を応答。０の場合は異常
	#define API_TXSIZE	64				// 送信用APIデータ長(32～255) シリアル送信最大長=API_TXSIZE-18バイト
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

void xbee_disp_1( int x ){
	char s[3];
	if(x<0){
		x = -x;
		s[0] = '-';
	}
	else s[0] = ' ';
	unsigned int x_u = (unsigned int)x;
	if		(x_u<10)	s[1]=((char)(x_u+0x30));
	else			s[1]='X';
	s[2]='\0';
	xbee_putstr(s);
}

void xbee_disp_2(int x ){
	char s[4];
	if(x<0){
		x = -x;
		s[0] = '-';
	}
	else s[0] = ' ';
	unsigned int x_u = (unsigned int)x;
	unsigned int y;
	if (x<100){
		y=x_u/10; s[1]=(char)(y+0x30); x_u-=(y*10);
		s[2]=(char)(x_u+0x30);
		s[3]='\0';
		if( s[1]=='0' ){
			s[1]=' ';
		}
		xbee_putstr( s );
	}else xbee_putstr("XX");
}

void xbee_disp_3(int x){
	char s[5];
	if(x<0){
		x = -x;
		s[0] = '-';
	}
	else s[0] = ' ';
	unsigned int x_u = (unsigned int)x;
	unsigned int y;
	if (x_u<1000){
		y=x_u/100; s[1]=(char)(y+0x30); x_u-=(y*100);
		y=x_u/10;  s[2]=(char)(y+0x30); x_u-= (y*10);
		s[3]=(char)(x_u+0x30);
		s[4]='\0';
		if( s[1]=='0' ){
			s[1]=' ';
			if( s[2]=='0' ){
				s[2]=' ';
			}
		}
		xbee_putstr( s );
	}else xbee_putstr("XXX");
}

void xbee_disp_5(int x){
	char s[7];
	if(x<0){
		x = -x;
		s[0] = '-';
	}
	else s[0] = ' ';
	unsigned int x_u = (unsigned int)x;
	unsigned int y;
	if (x_u<=65535){
		y=x_u/10000; s[1]=(char)(y+0x30); x_u-=(y*10000);
		y=x_u/1000;  s[2]=(char)(y+0x30); x_u-= (y*1000);
		y=x_u/100;   s[3]=(char)(y+0x30); x_u-=  (y*100);
		y=x_u/10;    s[4]=(char)(y+0x30); x_u-=   (y*10);
		s[5]=(char)(x_u+0x30);
		s[6]='\0';
		if( s[1]=='0' ){
			s[1]=' ';
			if( s[2]=='0' ){
				s[2]=' ';
				if( s[3]=='0' ){
					s[3]=' ';
					if( s[4]=='0' ){
						s[4]=' ';
					}
				}
			}
		}
		xbee_putstr( s );
	}else xbee_putstr("65535");
}

void xbee_putint(int x){
	if(-10 <x || x<10) xbee_disp_1(x);
	else if(-100 <x || x<100) xbee_disp_2(x);
	else if(-1000 <x || x<1000) xbee_disp_3(x);
	else xbee_disp_5(x);
}

void xbee_putdouble(double x){    //有効数字4桁まで表示 0.001<x<1000
	char s[7]; //符号(+-)+数字+小数点(.)+(\0)
	if(x<0){
		x = -x;
		s[0] = '-'; //先頭に-を挿入
	}
	else s[0] = ' ';

	unsigned int x_uint = 0;
	unsigned int x_1000 = 0; //x_unitの1000の位
	unsigned int x_100 = 0; //100以上の位
	unsigned int x_10 = 0; //10以上の位
	unsigned int x_1 = 0; //1以上の位
	unsigned int x_else = 0; //上記で表示されなかった残り
	unsigned int y;
	int x_int = 0;

	if(x<10){   //ex:1.567
		x = 1000*x;
		x_uint = (unsigned int)x;   //xを1000倍してunsigned intに変換
		y=x_uint/1000;  s[1]=(char)(y+0x30); x_uint-= (y*1000);
		s[2] = '.';
		y=x_uint/100;   s[3]=(char)(y+0x30); x_uint-=  (y*100);
		y=x_uint/10;    s[4]=(char)(y+0x30); x_uint-=   (y*10);
		s[5]=(char)(x+0x30);
		s[6]='\0';
		if( s[1]=='0' ){
			s[1]=' ';
			if( s[3]=='0' ){
				s[3]=' ';
				if( s[4]=='0' ){
					s[4]=' ';
				}
			}
		}
	xbee_putstr( s );
	}
	else if(x<100){  //ex:15.67
		x = 100*x;
		x_uint = (unsigned int)x;   //xを1000倍してunsigned intに変換
		y=x_uint/1000;  s[1]=(char)(y+0x30); x_uint-= (y*1000);
		y=x_uint/100;   s[2]=(char)(y+0x30); x_uint-=  (y*100);
		s[3] = '.';
		y=x_uint/10;    s[4]=(char)(y+0x30); x_uint-=   (y*10);
		s[5]=(char)(x+0x30);
		s[6]='\0';
		xbee_putstr( s );
	}
	else if(x<1000){ //ex:156.7
		x = 10*x;
		x_uint = (unsigned int)x;   //xを1000倍してunsigned intに変換
		y=x_uint/1000;  s[1]=(char)(y+0x30); x_uint-= (y*1000);
		y=x_uint/100;   s[2]=(char)(y+0x30); x_uint-=  (y*100);
		y=x_uint/10;    s[3]=(char)(y+0x30); x_uint-=   (y*10);
		s[4] = '.';
		s[5]=(char)(x+0x30);
		s[6]='\0';
		xbee_putstr( s );
	}
	else{ //ex:1567
		x_int = (int)x;
		xbee_putint(x_int);
	}
}
