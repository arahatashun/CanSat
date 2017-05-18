/***************************************************************************************
Bluetoothモジュール RN-42XVP用の通信ドライバ
Ver 1.1 for Raspberry Pi 2016/01/22

                                                Copyright (c) 2013-2016 Wataru KUNINO
                                                http://www.geocities.jp/bokunimowakaru/
***************************************************************************************/

#include "../libs/15term.c"                     // RFCOMM接続を行うライブラリの組み込み
#define RX_MAX  64                              // 受信データの最大サイズ
extern char rx_data[RX_MAX];                    // 受信データの格納用の文字列変数

/* シリアルの受信バッファを消去する関数 */
void bt_rx_clear(){
    while( read_serial_port() ){                // 受信データが残っている場合
        usleep(2000);                           // 受信中を考慮し、2ms待ち時間を付与
    }
}

/* コマンドの受信を行う関数 */
int bt_rx(void){
    int i,loop=50;                              // 変数iは受信したデータのサイズ
    
    for(i=0;i<(RX_MAX-1);i++) rx_data[i]='\0';  // 変数の初期化 memset(rx_data,0,RX_MAX)
    i=0;                                        // 受信済データの数の初期化
    while(loop>0){                              // 50回、くりかえす。
        loop--;
        rx_data[0] = read_serial_port();        // 受信データを読取り、変数rx_data[0]へ
        if( rx_data[0] ){                       // 何らかの受信データ(応答)があった場合
            for(i=1;i<(RX_MAX-2);i++){          // 受信データの変数の数だけ繰り返す
                rx_data[i]=read_serial_port();  // 受信データを保存する
                if( rx_data[i]==0 ) break;      // 受信データ無しの時にforループを抜ける
                usleep(2000);                   // 待ち時間を付与
            }
            bt_rx_clear();                      // シリアルの受信バッファを消去する
            loop=0;                             // whileループを抜けるためにloop値を0に
        }else usleep(10000);                    // 応答待ち
    }
    return(i);                                  // 受信したデータの大きさを戻り値にする
}

/* コマンド(cmd)の送受信を行う関数 */
int bt_cmd(char *cmd){
    bt_rx_clear();                              // シリアルの受信バッファを消去する
    write(ComFd, cmd, strlen(cmd) );            // 文字列変数cmdのデータを送信する
    if(strcmp(cmd,"$$$")) write(ComFd,"\n",1);  // $$$以外の時は改行コードを付与する
    return(bt_rx());                            // 送信後のRN-42の応答値(結果)を受信する
}

/* コマンドモード(mode)に入るための送受信を行う関数 */
int bt_cmd_mode(char mode){
    int i;
    char cmd[4];
    if( bt_cmd("GK") > 0 ){                     // コマンドに応答があった場合で、かつ、
        if( rx_data[0]=='1') bt_cmd("K,");      // ネットワーク接続されている場合は切断
        else bt_cmd("---");                     // 接続していない時はコマンドモード解除
        sleep(1);                               // (RN-42が応答するまでの)1秒の待ち時間
    }
    bt_rx_clear();                              // シリアルの受信バッファを消去する
    for(i=0;i<3;i++) cmd[i]=mode;               // コマンドモードに入る命令「$$$」を作成
    cmd[3]='\0';                                // 文字列の終端(念のための代入)
    write(ComFd, cmd, 3 );                      // コマンドモードに入る命令を実行
    i = bt_rx();                                // 結果を受信(正常なら「CMD」が返る)
    if( i>=5 ){                                 // 何らかの応答があった場合
        if( rx_data[i-5]=='C' && rx_data[i-4]=='M' && rx_data[i-3]=='D'){
            return(1);                          // 成功（CMDの応答があった時）
        }
    }
    return(0);                                  // 失敗（CMDの応答が無かった時）
}

/* コマンド(cmd)を期待の応答(res)が得られるまで永続的に発行し続ける関数 */
void bt_repeat_cmd(char *cmd, char *res, int SizeOfRes){
    int i=0,j;
    bt_rx_clear();                              // シリアルの受信バッファを消去する
    if(SizeOfRes >= RX_MAX)SizeOfRes=RX_MAX-1;  // 文字制限の調整
    do{
        if( i==0 ) bt_cmd(cmd);                 // コマンドの発行（10秒に1回）
        if( ++i > 10 ) i=0;                     // iのインクリメント
        sleep(1);                               // 1秒間の応答待ち
        if( bt_rx() ){                          // 受信データを取得
            if( SizeOfRes == 0 ) i=-1;          // SizeOfResが0の時は受信内容の確認なし
            for(j=0; j <= SizeOfRes ; j++ ){    // 内容を確認
                if( res[j] == '\0' ){           // 期待文字がすべて一致した
                    i=-1;                       // do-whileループを抜ける
                    break;                      // 本forループを抜ける
                }
                if( res[j] != rx_data[j] || rx_data[j] == '\0') break;
            }                                   // 不一致があればforループを抜ける
            if( j == SizeOfRes ) i=-1;          // 
        }
    }while( i >= 0 );                           // 期待の応答があるまで繰り返し
}

/* エラー表示用の関数 */
void bt_error(char *err){
    char s[17];
    int i;
    for(i=0;i<16;i++){
        if(rx_data[i]=='\0') break;
        if(isprint(rx_data[i])) s[i]=rx_data[i];
        else s[i]=' ';
    }
    s[i]='\0';
    if(i==0) sprintf(s,"No RX data");
    bt_cmd("K,");                               // 通信の切断
    bt_cmd("---");                              // コマンドモードの解除
    fprintf(stderr,"%s\nPlease RESTART\n",err); // 再起動依頼の表示
    exit(-1);
}

/* ローカルMaster機の設定用 */
void bt_init_local(void){
    printf("Config BT \n"); 
    if( !bt_cmd_mode('$') ){                    // ローカルコマンドモードへの移行を実行
        bt_error("Config FAILED");
    }
    bt_cmd("SF,1");                             // 工場出荷時の設定に戻す
    bt_cmd("SM,1");                             // BluetoothのMasterデバイスに設定する
    bt_cmd("SA,4");                             // PINペアリング方式に変更する。
    bt_cmd("SO,%");                             // 接続・切断時にメッセージを表示する
    bt_cmd("ST,255");                           // コマンドモードの時間制限を解除する
    bt_cmd("R,1");                              // 再起動
    printf("DONE\n");
    sleep(1);                                   // 再起動待ち

    /* デバイス探索の実行 */
    if( !bt_cmd_mode('$') ){                    // ローカルコマンドモードへの移行を実行
        bt_error("FAILED to open");
    }
    printf("Inquiry \n");
    bt_repeat_cmd("I","Found",5);               // デバイスが見つかるまで探索を繰り返す
    printf("Found\n");
    while( bt_rx()==0 );                        // アドレスの取得待ち
    
    /* ペアリングの実行 */
    printf("Pairing \n");
    bt_cmd("SR,I");                             // 発見したデバイスのアドレスを保存
    bt_repeat_cmd("C","%CONNECT",8);            // 接続するまで接続コマンドを繰り返す
    printf("DONE\n");
    sleep(1);                                   // 接続後の待ち時間
    
    /* リモート機の設定 */
    printf("RemoteCnf \n");
    if( !bt_cmd_mode('$') ){                    // ローカルコマンドモードへの移行
        bt_error("RemoteCnf Failed");
    }
    bt_cmd("ST,255");                           // コマンドモードの時間制限を解除する
    bt_cmd("R,1");                              // 再起動
    printf("DONE\n");
    sleep(1);                                   // 再起動待ち
}

/* リモートSlave機との接続 */
int bt_init(char *mac){
    printf("Bluetooth Remote\n");               // タイトル文字を表示
    if(open_rfcomm(mac) < 0){                   // Bluetooth SPP RFCOMM 接続の開始
        bt_error("Bluetooth Open ERROR");       // エラー表示後に異常終了
        return -1;                              // 異常終了
    }
    return 0;                                   // 正常終了
}

/* RFCOMM通信の切断処理 */
void bt_close(void){
    close_rfcomm();                             // Bluetooth SPP RFCOMM 通信の切断処理
}
