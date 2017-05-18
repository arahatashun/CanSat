/*
�g �� �b �L �[ �R �[ �h �l �b �g 

Linux��kbhit�֐��쐬 (C����)
Linux���ł�C����J���ŁAWindows�ł����Ƃ����kbhit()���Č������R�[�h�ł��B
(�� �p��T�C�g�ɓ]�����Ă����\�[�X�ɁA�ꕔ�C�������������̂ł��B)

�E�E�E������g����������Ȃ��̂ŁA�������Ă�u���Ă����܂��O�O�G�j

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
