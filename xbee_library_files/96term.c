/**************************************************************************************
9600bps Term for Raspberry Pi

                                                Copyright (c) 2015-2016 Wataru KUNINO
                                                http://www.geocities.jp/bokunimowakaru/
***************************************************************************************/
#include <stdio.h>                                  // 標準入出力用
#include <stdlib.h>                                 // system関数用
#include <fcntl.h>                                  // シリアル通信用(Fd制御)
#include <termios.h>                                // シリアル通信用(端末IF)
#include <string.h>                                 // strncmp,bzero用
#include <sys/time.h>                               // fd_set,select用
#include <unistd.h>                                 // read,usleep用
#include <ctype.h>                                  // isxdigit用

typedef unsigned char byte;
static int ComFd;                                   // シリアル用ファイルディスクリプタ
static struct termios ComTio_Bk;                    // 現シリアル端末設定保持用の構造体

int open_serial_port(char *modem_dev){
    struct termios ComTio;                          // シリアル端末設定用の構造体変数
    speed_t speed = B9600;                          // 通信速度の設定 9600 bps
    ComFd=open(modem_dev, O_RDWR|O_NONBLOCK);       // シリアルポートのオープン
    if(ComFd >= 0){                                 // オープン成功時
    //  printf("com=%s\n",modem_dev);               // 成功したシリアルポートを表示
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

int gets_serial_port( char *data, int len ){
    int i;
    for(i=0;i<len-1;i++){
        data[i]=read_serial_port();
        if(data[i] == 0 || data[i] == '\r' || data[i] == '\n' ){
            break;
        }
    }
    data[i]=0;
    return i;
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

