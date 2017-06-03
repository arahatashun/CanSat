// flight_integration.c

#include<stdio.h>
#include<time.h>
#include<gps.h>
// #include<xbeeuart.h> とりあえずxbee関係はコメントアウトしてあります
#include<WiringPi.h>
#include<unistd.h>
#include"luxsensor.h"
#include"gut.h"
#include"ring_buffer.h"
#include"flight.h"

time_t start_all_time; //全体の開始時刻 グローバル変数
static loc_t flight_gps_data; //gpsデータ確認用 integration_kndのdataとの衝突を回避

Queue *gpsflight_lat_ring = NULL; //緯度データ
Queue *gpsflight_lon_ring = NULL; //経度データ
Queue *gpsflight_alt_ring = NULL; //高度データ
File *fp; //ファイル型ポインタfp

static const int gps_ring_len = 10; //GPSの値を格納するリングバッファの長さ
static const int timeout_lux = 60; //光センサー放出判定タイムアウト時間(min)
static const int timeout_gpsstable = 80; //上記に失敗した場合、gpsの三軸安定で着地判定するが、そのタイムアウト時間(min)
static const int timeout_altsatble = 100; //着地判定(gps高度)タイムアウト時間(min)

//以下フラグ
static int lux_timeout_flag = 0; //放出判定(luxセンサー)タイムアウトフラグ タイムアウトで1
static int release_complete = 0; //放出判定フラグ 放出判定で1
static int landing_complete = 0; //着地判定フラグ 着地判定で1

//関数宣言
static int flightsensor_setup();
static int gps_3axisstable();
static int gps_altstable();


int timer_setup(){
  //制御開始時刻を取得、画面に表示

  time(&start_all_time);
  printf("start_time: %s\n", ctime(&timer));
  return 0;
}

int timestamp(){
  //start_all_timeからの経過時間をtimestamp
  time_t current_time;//時間を取得
	time(&current_time);
	int delta_time = (int)difftime(current_time,start_all_time);
  int delta_time_hour = delta_time/3600;
  int delta_time_min = delta_time/60 - (60*delta_time_hour);
  int delta_time_sec = delta_time - (3600*delta_time_hour + 60*delta_time_min);
	printf("OS timestamp %dh:%dm:%ds\n",delta_time_hour,delta_time_min,delta_time_sec);
  /* xbee用表示関数
  xbee_print_int(delta_time_hour); xbee_print_char("h:");
  xbee_print_int(delta_time_min); xbee_print_char("m:");
  xbee_print_int(delta_time_sec); xbee_print_char("s\n");
  */
  return 0;
}

int get_difftime(){
  //start_all_timeからの経過時間(min)を取得して返す
  time_t current_time;//時間を取得
	time(&current_time);
	double delta_time = difftime(current_time,start_time);
  return (int)delta_time/60; //minに直す
}

double write_status(int sequence){
  //ステータスファイルにシーケンスデータを書き込む
  if((fp = fopen("status","w")) == NULL){
    //ファイルが開けない
    printf("cannot open sequence file");
    return -1;
  }
  else{
    fprintf(fp, "%f\n", &sequence);
    printf("write_statusfile;sequence:%f\n", sequence);
    fclose(fp);
    return 0;
  }
}

double read_status(){
  //ファイルに最後に書かれたシーケンスを読み取る。シーケンス番号をreturnする
  if((fp = fopen("status","r")) == NULL){
    printf("nothing written in file\n");
    return -1;
  }
  else{
    int last_sequence; //最後に到達したシーケンス番号
    fscanf(fp, "%f", last_sequence);
    printf("read_statusfile;start from sequence:%d\n", last_sequence);
    return last_sequence;
  }
}

static int flightsensor_setup(){
  luxsensor_initializer();
  // xbee_initializer();
  gps_init();
  return 0;
}

static int gps_3axisstable(){
  //GPS3軸の値が安定で1を返す、不安定で0を返す
  gpsflight_lat_ring = make_queue(gps_ring_len);
  gpsflight_lon_ring = make_queue(gps_ring_len);
  gpsflight_alt_ring = make_queue(gps_ring_len);

  while(!is_full(flight_gps_data)){
  gps_location(&flight_gps_data);
  enqueue(gpsflight_lat_ring,flight_gps_data.latitude);
  enqueue(gpsflight_lon_ring,flight_gps_data.longitude);
  enqueue(gpsflight_alt_ring,flight_gps_data.altitude);
  sleep(2);
  }

  double INF = 10000;
  double minlat=INF,maxlat=0;
  double minlon=INF,maxlon=0;
  double minalt=INF,maxalt=0;

  for(i=0,i<gps_ring_len,i++){
    double lati=0;
    double loni=0;
    double alti=0;
    lati=dequeue(gpsflight_lat_ring);
    loni=dequeue(gpsflight_lon_ring);
    alti=dequeue(gpsflight_alt_ring);
    if(lati<minlat) minlat = lati;
    if(lati>maxlat) maxlat = lati;
    if(loni<minlon) minlon = loni;
    if(loni>maxlon) maxlon = loni;
    if(alti<minalt) minalt = alti;
    if(alti>maxalt) maxalt = alti;
  }
  double abslat = fabs(minlat,maxlat);
  double abslon = fabs(maxlon,minlon);
  double absalt = fabs(maxalt,minalt);
  if(abslat<0.00003 && abslot<0.00003 && absalt<3){
      return 1;
  }
  sleep(2);
  return 0;
}

static int gps_altstable(){
  //GPS高度の値が安定で1を返す、不安定で0を返す
  gpsflight_alt_ring = make_queue(gps_ring_len);

  while(!is_full(flight_gps_data)){
  gps_location(&flight_gps_data);
  enqueue(gpsflight_alt_ring,flight_gps_data.altitude);
  sleep(2);
  }

  double INF = 10000;
  double minalt=INF,maxalt=0;

  for(i=0,i<gps_ring_len,i++){
    double alti=0;
    alti=dequeue(gpsflight_alt_ring);
    if(alti<minalt) minalt = alti;
    if(alti>maxalt) maxalt = alti;
  }
  double absalt = fabs(maxalt,minalt);
  if(abslat<0.00003 && abslot<0.00003 && absalt<3){
      return 1;
  }
  sleep(2);
  return 0;
}

int release(){
  flightsensor_setup();

  int islight_counter = 0;

  while(!release_complete){
    //放出判定が出るまで繰り返す(timeoutによる抜け出しあり)
    if(is_lightcounter == 9){
      //10回連続でislightが1
      release_complete = 1;
      timestamp();
      printf("release complete\n");
      write_status(1.0)
    }
    else if(get_difftime() > timeout_lux){
      lux_timeout_flag = 1;
      printf("timeout_lux;release_complete\n");
      write_status(1.1)
      break;
    }
    else if(islight() == 1) islight_counter++;
    else islightcounter = 0; //islightで暗い判定 カウンターを0に戻してやり直し
    sleep(2); //2秒間空ける
  }
  return 0;
}
  //以上で放出判定完了

int landing(){
  if(lux_timeout_flag == 1 || read_status() == 1.1){
    //時間切れした場合の処理 lux_timeoutフラグまたはstatusファイルの読み込みで判断
    while(!landing_complete){
      if(get_difftime() > timeout_gpsstable){
        timestamp();
        printf("timeout_gpsstable;landing_complete\n");
        landing_complete = 1;
        break;
      }
      else if(gps_3axisstable()){
        timestamp();
        printf("landing_complete(judged by 3 axis)\n")
        landing_complete = 1;
      }
      else;
    }
  }
  else{
    //正常に照度センサーで放出判定できた場合の処理
    while(!landing_complete){
      if(get_difftime() > timeout_altstable){
        timestamp();
        printf("timeout_altstable;landing_complete\n");
        landing_complete = 1;
        break;
      }
      else if(gps_altstable()){
        if(gps_altstable()){
        //ダブルチェック
        timestamp();
        printf("landing_complete(judged by 3 axis)\n");
        landing_complete = 1;
        }
      }
      else;
    }
  }
  write_status(2);
  return 0;
}
//以上で着地判定終了

int casing_open(){
  cut_initializer();
  cut();
  timestamp();
  printf("casing_open\n");
  write_status(3);
  return 0;
}
//以上でケーシング展開完了

int main(){
  timer_setup(); //制御開始時刻を取得
  write_status(0); //シーケンス0(制御スタート)をファイルに書き込む
  release();
  landing();
  casing_open();
}
