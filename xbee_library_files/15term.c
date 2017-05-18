/**************************************************************************************
Ichigo Term for Raspberry Pi

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

static int ComFd;                                   // シリアル用ファイルディスクリプタ
static struct termios ComTio_Bk;                    // 現シリアル端末設定保持用の構造体

int open_serial_port(){
    struct termios ComTio;                          // シリアル端末設定用の構造体変数
    speed_t speed = B115200;                        // 通信速度の設定
    char modem_dev[15]="/dev/ttyUSBx";              // シリアルポートの初期値
    int i;
    for(i=12;i>=10;i--){    // i>=10に修正(rfcommのみに使用)
        if(i>=10) snprintf(&modem_dev[5],8,"rfcomm%1d",i-10);   // ポート探索(rfcomm0-2)
        else if(i>=0) snprintf(&modem_dev[5],8,"ttyUSB%1d",i);  // ポート探索(USB0～9)
        else snprintf(&modem_dev[5],8,"ttyAMA0");   // 拡張IOのUART端子に設定
        ComFd=open(modem_dev, O_RDWR|O_NONBLOCK);   // シリアルポートのオープン
        if(ComFd >= 0){                             // オープン成功時
            printf("com=%s\n",modem_dev);           // 成功したシリアルポートを表示
            tcgetattr(ComFd, &ComTio_Bk);           // 現在のシリアル端末設定状態を保存
            ComTio.c_iflag = 0;                     // シリアル入力設定の初期化
            ComTio.c_oflag = 0;                     // シリアル出力設定の初期化
            ComTio.c_cflag = CLOCAL|CREAD|CS8;      // シリアル制御設定の初期化
            ComTio.c_lflag = 0;                     // シリアルローカル設定の初期化
            bzero(ComTio.c_cc,sizeof(ComTio.c_cc)); // シリアル特殊文字設定の初期化
            cfsetispeed(&ComTio, speed);            // シリアル入力の通信速度の設定
            cfsetospeed(&ComTio, speed);            // シリアル出力の通信速度の設定
            ComTio.c_cc[VMIN] = 0;                  // リード待ち容量0バイト(待たない)
            ComTio.c_cc[VTIME] = 0;                 // リード待ち時間0.0秒(待たない)
            tcflush(ComFd,TCIFLUSH);                // バッファのクリア
            tcsetattr(ComFd, TCSANOW, &ComTio);     // シリアル端末に設定
            break;                                  // forループを抜ける
        }
    }
    return ComFd;
}

int open_rfcomm(char *mac){
/*
    Bluetooth RFCOMMプロファイル接続を行います。
    macは文字列が入ります。以下のような18バイト17文字の構成としてください。

    char mac[] = "xx:xx:xx:xx:xx:xx";   // xxは2桁の16進数

    ※セキュリティ対策のため、書式が異なると実行されません。
*/
    char s[64];
    int i=0,ret=-1;
    
    if( strlen(mac) == 17){                         // セキュリティチェック
        sprintf(s,"sudo /usr/bin/rfcomm connect /dev/rfcomm %s &",mac);
        for(i=2;i<17;i+=3) mac[i]=':';              // セキュリティ対策
        for(i=0;i<17;i+=3){
            if(isxdigit(mac[i])==0 || isxdigit(mac[i+1])==0 ) break;
        }                                           // セキュリティチェック(16進数)
    }
    if(i==18){
        printf("[%s]\n",s);
        system(s);
        for(i=0;i<15;i++){
            sleep(1);
        //  printf("( %d sec.) \n\x1b\x5b\x41",14-i);
            ret=open_serial_port();
            if(ret >= 0) break;
        }
    }else fprintf(stderr,"Invalid Mac Address ERROR (i=%d,%c%c)\n",i,mac[i],mac[i+1]);
    return ret;
}

char read_serial_port(void){
    char c='\0';                                    // シリアル受信した文字の代入用
    fd_set ComReadFds;                              // select命令用構造体ComReadFdを定義
    struct timeval tv;                              // タイムアウト値の保持用
    FD_ZERO(&ComReadFds);                           // ComReadFdの初期化
    FD_SET(ComFd, &ComReadFds);                     // ファイルディスクリプタを設定
    tv.tv_sec=0; tv.tv_usec=10000;                  // 受信のタイムアウト設定(10ms)
    if(select(ComFd+1, &ComReadFds, 0, 0, &tv)) read(ComFd, &c, 1); // データを受信
    usleep(5000);                                   // 5msの(IchigoJam処理)待ち時間
    return c;                                       // 戻り値＝受信データ(文字変数c)
}

int close_serial_port(void){
    tcsetattr(ComFd, TCSANOW, &ComTio_Bk);
    return close(ComFd);
}

int close_rfcomm(){
/*
    open_rfcommで開いたプロセスをkillするモジュールです。
    戻り値はkillを実行した時のプロセスIDです。
    killが実行できなかった場合は0を応答します。
*/
    FILE *fp;
    char s[]="pidof /usr/bin/rfcomm |awk '{print $1;}'";
    int id;
    
    close_serial_port();
    fp=popen(s,"r");
    s[0]='\0';
    if(fp){
        if( feof(fp)==0 ) fgets(s,sizeof(s),fp);    // 終端では無い時にfpから値を読む
        fclose(fp);
    }
    
    id=atoi(s);                                     // atoiはセキュリティ対策と改行対策
    if(id>0){
        sprintf(s,"sudo kill %d",id);
        printf("[%s]\n",s);
        system(s);
    }
    return id;
}
