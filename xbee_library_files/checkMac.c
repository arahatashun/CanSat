/***************************************************************************************
本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

MACアドレスのフォーマットを確認する
xx:xx:xx:xx:xx:xx

                                                       Copyright (c) 2016 Wataru KUNINO
                                                 http://www.geocities.jp/bokunimowakaru/
***************************************************************************************/
#include <string.h>                                             // strnlen命令に使用
#include <ctype.h>                                              // isxdigit用

int checkMac(char *mac){
    int i;
    if(strlen(mac) != 17){                                      // MACアドレス文字長確認
        fprintf(stderr,"Invalid MAC length (%d)\n",strlen(mac));  // 入力誤り表示
        return -1;                                              // 終了
    }
    for(i=0;i<17;i++){                                          // MACアドレス形式の
        if( (i+1)%3 == 0 ){                                     // 3,6,9,12,15文字目が
            if( mac[i] != ':' ) break;                          // 「:」であることを確認
        }else{                                                  // その他の文字が
            if(isxdigit(mac[i])==0) break;                      // 16進数である事を確認
            if(mac[i]>='a') mac[i] -= 'a'-'A';					// 小文字の時は大文字に
        }
    }
    if(i!=17){                                                  // for中にBreakした時
        fprintf(stderr,"Invalid MAC Format (%s,%d)\n",mac,i);   // 形式誤り表示
        return -1;                                              // 終了
    }
    return 0;
}
