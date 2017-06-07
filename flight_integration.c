// flight_integration.c

#include <stdio.h>
#include <time.h>
#include <gps.h>
// #include<xbeeuart.h> とりあえずxbee関係はコメントアウトしてあります
#include <wiringPi.h>
#include <unistd.h>
#include <math.h>
#include "luxsensor.h"
#include "gut.h"
#include "ring_buffer.h"
#include "flight_integration.h"

time_t start_all_time; //全体の開始時刻 グローバル変数
static loc_t flight_gps_data; //gpsデータ確認用 integration_kndのdataとの衝突を回避

Queue *gpsflight_lat_ring = NULL; //緯度データ
Queue *gpsflight_lon_ring = NULL; //経度データ
Queue *gpsflight_alt_ring = NULL; //高度データ
FILE *statusfp; //ファイル型ポインタstatusfp

static const int GPS_RING_LEN = 10; //GPSの値を格納するリングバッファの長さ
static const int TIMEOUT_LUX = 60; //光センサー放出判定タイムアウト時間(min)
static const int TIMEOUT_GPSSTABLE = 80; //上記に失敗した場合、gpsの三軸安定で着地判定するが、そのタイムアウト時間(min)
static const int TIMEOUT_ALTSTABLE = 100; //着地判定(gps高度)タイムアウト時間(min)

static const int STS_INIT = 0; //ステータス0 最初
static const int STS_RELEASECOMPLETE = 1.0; //ステータス1 放出判定終了
static const int STS_RELEASETIMEOUT = 1.1; //ステータス1.1 放出判定時間切れ
static const int STS_LANDINGCOMPLETE = 2; //ステータス2 着地判定終了
static const int STS_CASINGOPENCOMPLETE = 3; //ステータス3 ケーシング展開終了

static const int ABSLAT_THRESHOLD = 0.00003; //GPS緯度情報安定判定閾値
static const int ABSLON_THRESHOLD = 0.00003; //GPS経度情報安定判定閾値
static const int ABSALT_THRESHOLD = 3; //GPS高度情報安定判定閾値

static const int GPS_3AXIS_INTERVAL = 2; //GPS高度取得間隔(gps_3axisstable内) second
static const int GPS_ALT_INTERVAL = 2; //GPS高度取得間隔(gps_altstable内) second

//以下フラグ
int lux_timeout_flag = 0; //放出判定(luxセンサー)タイムアウトフラグ タイムアウトで1
static int release_complete = 0; //放出判定フラグ 放出判定で1
static int landing_complete = 0; //着地判定フラグ 着地判定で1

//関数宣言
static int flightsensor_setup();
static int gps_3axisstable();
static int gps_altstable();
static int landing_timeout_ver();
static int landing_lux_ver();
static double calc_variation(Queue *gpsflight_hoge_ring);

int timer_setup(){
	//制御開始時刻を取得、画面に表示
	time(&start_all_time);
	printf("start_time: %s\n", ctime(&start_all_time));
	return 0;
}

//printfするだけの関数
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
	double delta_time = difftime(current_time,start_all_time);
	return (int)delta_time/60; //minに直す
}

double write_status(double sequence){
	//ステータスファイルにシーケンスデータを書き込む
	if((statusfp = fopen("status","w")) == NULL) {
		//ファイルが開けない
		printf("cannot open sequence file\n");
		return -1;
	}
	else{
		fprintf(statusfp, "%lf\n",sequence);
		printf("write_statusfile;sequence:%lf\n", sequence);
		fclose(statusfp);
		return 0;
	}
}

double read_status(){
	//ファイルに最後に書かれたシーケンスを読み取る。シーケンス番号をreturnする
	if((statusfp = fopen("status","r")) == NULL) {
		printf("nothing written in file\n");
		return -1;
	}
	else{
		double last_sequence; //最後に到達したシーケンス番号
		fscanf(statusfp,"%lf",&last_sequence);
		fclose(statusfp);
		printf("read_statusfile;start from sequence:%lf\n", last_sequence);
		return last_sequence;
	}
}

static int flightsensor_setup(){
	luxsensor_initializer();
	// xbee_initializer();
	gps_init();
	return 0;
}

static double calc_variation(Queue *gpsflight_hoge_ring)
{
	double INF = 10000;
	double minhoge = INF;
	double maxhoge = 0;
	for(int i=0; i<queue_length(gpsflight_hoge_ring); i++)
	{
		double hogei=0;
		hogei=dequeue(gpsflight_hoge_ring);
		if(hogei<minhoge) minhoge = hogei;
		if(hogei>maxhoge) maxhoge = hogei;
	}
	double abshoge = INF;//0にするよりも大きくする方があとでtheresholdと比較するので適切
	double abshoge = fabs(maxhoge - minhoge);
	return abshoge;
}

//GPS3軸の値が安定で1を返す、不安定で0を返す
static int gps_3axisstable(){
	//ring_bufferを作る
	gpsflight_lat_ring = make_queue(GPS_RING_LEN);
	gpsflight_lon_ring = make_queue(GPS_RING_LEN);
	gpsflight_alt_ring = make_queue(GPS_RING_LEN);

	while(!is_full(gpsflight_lat_ring)) {
		gps_location(&flight_gps_data);
		//以下落下中ログデータ、時間緯度経度高度を送る
		timestamp();
		printf("latitude:%f longitude:%f altitude:%f\n",
		       flight_gps_data.latitude,flight_gps_data.longitude,flight_gps_data.altitude);
		//以上ログデータ
		enqueue(gpsflight_lat_ring,flight_gps_data.latitude);
		enqueue(gpsflight_lon_ring,flight_gps_data.longitude);
		enqueue(gpsflight_alt_ring,flight_gps_data.altitude);
		sleep(2);
	}

	if(calc_variation(&gpsflight_lat_ring)<ABSLAT_THRESHOLD &&
	   calc_variation(&gpsflight_lon_ring)<ABSLON_THRESHOLD &&
	   calc_variation(&gpsflight_alt_ring)<ABSALT_THRESHOLD)
	{
		return 1;
	}
	sleep(2);
	return 0;
}

static int gps_altstable(){
	//GPS高度の値が安定で1を返す、不安定で0を返
	printf("enter_gpsaltsable\n");
	gpsflight_alt_ring = make_queue(GPS_RING_LEN);

	while(!is_full(gpsflight_alt_ring)) {
		gps_location(&flight_gps_data);
		//以下落下中ログデータ、時間緯度経度高度を送る
		timestamp();
		printf("latitude:%f longitude:%f altitude:%f\n",
		       flight_gps_data.latitude,flight_gps_data.longitude,flight_gps_data.altitude);
		//以上ログデータ
		enqueue(gpsflight_alt_ring,flight_gps_data.altitude);
		sleep(GPS_ALT_INTERVAL);
	}
	if(calc_variation(&gpsflight_alt_ring)<ABSALT_THRESHOLD) {
		return 1;
	}
	sleep(2);
	return 0;
}

/*
   照度センサータイムアウトの場合は、より慎重に着地を判定するために3軸安定を使い、放出判定が成功してる場合は、
   高速化のためと着地後風で飛ばされることも考慮して高度情報の安定化のみを判定基準にしてる
   (上空で安定化してしまった場合に備えて、高度が一定値以下も条件に入れるべきかも)
 */

int release(){
	flightsensor_setup();

	int islight_counter = 0;//islight()return 1した回数

	while(release_complete!=1) {
		//放出判定が出るまで繰り返す(timeoutによる抜け出しあり)
		if(islight_counter == 9) {
			//10回連続でislightが1
			release_complete = 1;
			timestamp();
			printf("release complete\n");
			write_status(STS_RELEASECOMPLETE);
		}
		else if(get_difftime() > TIMEOUT_LUX) {
			lux_timeout_flag = 1;
			printf("TIMEOUT_LUX;release_complete\n");
			write_status(STS_RELEASETIMEOUT);
			break;
		}
		else if(islight() == 1)
		{
			islight_counter++;
		}
		else
		{
			islight_counter = 0; //islightで暗い判定 カウンターを0に戻してやり直し
		}
		sleep(2); //2秒間空ける
	}
	return 0;
}
//以上で放出判定完了


static int landing_timeout_ver(){
	//時間切れした場合の処理 lux_timeoutフラグまたはstatusファイルの読み込みで判断
	printf("enter timeout_ver");
	while(!landing_complete) {
		if(get_difftime() > TIMEOUT_GPSSTABLE) {
			timestamp();
			printf("TIMEOUT_GPSSTABLE;landing_complete\n");
			//着地地点ログ
			printf("landing point: latitude:%f longitude:%f alttitude:%f\n",
			       flight_gps_data.latitude,flight_gps_data.longitude,flight_gps_data.altitude);
			landing_complete = 1;
			break;
		}
		else if(gps_3axisstable()) {
			timestamp();
			printf("landing_complete(judged by 3 axis)\n");
			//着地地点ログ
			printf("landing point: latitude:%f longitude:%f alttitude:%f\n",
			       flight_gps_data.latitude,flight_gps_data.longitude,flight_gps_data.altitude);
			landing_complete = 1;
		}
	}
	return 0;
}

static int landing_lux_ver(){
	//正常に照度センサーで放出判定できた場合の処理
	while(!landing_complete) {
		if(get_difftime() > TIMEOUT_ALTSTABLE) {
			timestamp();
			printf("TIMEOUT_ALTSTABLE;landing_complete\n");
			//着地地点ログ
			printf("landing point: latitude:%f longitude:%f alttitude:%f\n",
			       flight_gps_data.latitude,flight_gps_data.longitude,flight_gps_data.altitude);
			landing_complete = 1;
			break;
		}
		else if(gps_altstable()) {
			if(gps_altstable()) {
				//ダブルチェック
				timestamp();
				printf("landing_complete(judged by altitude)\n");
				//着地地点ログ
				printf("landing point: latitude:%f longitude:%f alttitude:%f\n",
				       flight_gps_data.latitude,flight_gps_data.longitude,flight_gps_data.altitude);
				landing_complete = 1;
			}
		}
	}
	return 0;
}

int landing(){
	printf("started landing phase:lux_flag;%d\n",lux_timeout_flag);
	if(lux_timeout_flag == 1 || read_status() == STS_RELEASETIMEOUT) {
		//時間切れした場合の処理 lux_timeoutフラグまたはstatusファイルの読み込みで判断
		landing_timeout_ver();
	}
	else{
		//正常に照度センサーで放出判定できた場合の処理
		landing_lux_ver();
	}
	write_status(STS_LANDINGCOMPLETE);
	return 0;
}
//以上で着地判定終了

int casing_open(){
	printf("started casing_open phase\n");
	cut_initializer();
	cut();
	timestamp();
	printf("casing_open\n");
	write_status(STS_CASINGOPENCOMPLETE);
	return 0;
}
//以上でケーシング展開完了

int main(){
	timer_setup(); //制御開始時刻を取得
	write_status(STS_INIT); //シーケンス0(制御スタート)をファイルに書き込む
	release();
	landing();
	casing_open();
	return 0;
}
