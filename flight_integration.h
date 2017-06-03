//flight_integration.h

#ifdef FLIGHT_H
#define FLIGHT_H

int timer_setup(); //制御開始時間(start_all_time)を取得
int timestamp(); //start_all_timeからの経過時間を表示
int get_difftime(); //start_all_timeからの経過時間(min)を取得
double write_status(); //"status"テキストファイルにシーケンスを書き込む
double read_status(); //"status"ファイルからシーケンスを読み取る
int release(); //放出判定までの処理
int landing(); //着地判定までの処理
int casing_open(); //ケーシング展開までの処理

#endif
