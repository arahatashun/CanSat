#ifndef COMPASS_H
#define COMPASS_H

int compass_initializer();
int compass_get_angle();

/*以下は近藤が自分の実験用に勝手に追記しました。
*/
int compass_initializer_knd();
short get_xcompass();
short get_ycompass();
short get_zcompass();
#endif
