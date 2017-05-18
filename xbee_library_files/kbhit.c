/*
ト リ ッ キ ー コ ー ド ネ ッ ト 

Linuxでkbhit関数作成 (C言語)
Linux環境でのC言語開発で、Windowsでいうところのkbhit()を再現したコードです。
(※ 英語サイトに転がっていたソースに、一部修正を加えたものです。)

・・・いずれ使うかもしれないので、メモがてら置いておきます＾＾；）

http://tricky-code.net/mine/c/mc06linuxkbhit.php
*/

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

int kbhit(void){
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
