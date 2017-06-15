#include <stdio.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <gps.h>
#include "luxsensor.h"
#include "gut.h"
#include "ring_buffer.h"

//sequence_numの定義について
static const int START_SEQ = 1;
static const int RELEASE_COMPLETE_SEQ = 2; //放出判定正常終了
static const int RELEASE_TIMEOUT_SEQ = 3; //放出判定時間切れ
static const int LAND_COMPLETE_SEQ = 4;
static const int OPEN_CASE_SEQ = 6;//ケーシング展開終了
//タイムアウト時間 (分)
static const int TIMEOUT_LUX = 60; //光センサー放出判定
static const int TIMEOUT_ALT_STABLE = 40; //gps高度着地判定
static const int TIMEOUT_3_STABLE = 20; //gps三軸着地判定

//関数の返り値
static const int GPS_FLIGHT_RING = 10;//ring_bufferの長さ
static const int ALT_IS_LOW = 1;
static const int ALT_IS_HIGH = 0;
static const int GPS3_IS_STABLE = 1;
static const int GPS3_NOT_STABLE = 0;
static const int GPS_ALT_IS_STABLE = 1;
static const int GPS_ALT_NOT_STABLE = 0;
static const int TIME_NOT_OUT= 1;
static const int TIME_IS_OUT = 0;
static const int RELEASE_COMPLETE = 1;
static const int RELEASE_TIMEOUT = 0;
//THRESHOLD
static const float ABSLAT_THRESHOLD = 0.00005; //GPS緯度情報安定判定閾値
static const float ABSLON_THRESHOLD = 0.00005; //GPS経度情報安定判定閾値
static const int ABSALT_THRESHOLD = 5; //GPS高度情報安定判定閾値
static const int ALT_THRESHOLD = 100; //GPS高度情報一定値以下判定閾値(m)
static const int GPS_3AXIS_INTERVAL = 2; //GPS高度取得間隔(gps_3axisstable内) second
static const int GPS_ALT_INTERVAL = 2; //GPS高度取得間隔(gps_altstable内) second

typedef struct st_Sequence {
	int sequence_num;//前のシーケンス番号
	time_t last_time;//前のシーケンスの時間
}Sequence;


//前回のシーケンスの終了時刻と現在のシーケンスを取得
int read_sequence(Sequence*sequence2read)
{
	FILE *fp = fopen("sequence.txt","r");
	if(fp==NULL)
	{
		printf("Cannot open sequence.txt\n");
		return -1;
	}
	else
	{
		while(fscanf(fp,"%d:%ld\n",&(sequence2read->sequence_num),&(sequence2read->last_time))!= EOF)
		{
			;//pass
		}
		printf("read_sequence %d:%ld\n",sequence2read->sequence_num,sequence2read->last_time);
		fclose(fp);
	}
	return 0;
}

//シーケンス番号と時刻を書き込む
int write_sequence(Sequence *sequence2write,int seq_num2write)
{
	time_t tcurrent;
	time(&tcurrent);
	sequence2write->last_time = tcurrent;
	sequence2write->sequence_num = seq_num2write;
	printf("write sequence %d:%s\n",seq_num2write,ctime(&tcurrent));
	FILE *fp=fopen("sequence.txt","a");//追加書き込み
	if(fp==NULL)
	{
		printf("Cannot open sequence\n");
		return -1;
	}else{
		fprintf(fp,"%d:%ld\n",seq_num2write,tcurrent);
		printf("write sequence success\n");
		fclose(fp);
	}
	return 0;
}

//Queueの中の値の変動を返す
//TODO gpsゼロで帰ってくる対策
static double calc_variation(Queue *gpsflight_tmp_ring)
{
	double INF = 10000;
	double min = INF;
	double max = 0;
	int i;
	for(i = 0; i<queue_length(gpsflight_tmp_ring); i++)
	{
		double tmp=0;
		tmp=dequeue(gpsflight_tmp_ring);
		min = tmp<min ? tmp : min;
		max = tmp>max ? tmp : max;
	}
	double abstmp = INF;//0にするよりも大きくする方があとでtheresholdと比較するので適切
	abstmp = fabs(max - min);
	printf("VARIATION :%f\n",abstmp);
	return abstmp;
}


//ring_bufferを受け取り高度が閾値より低いか判定
//TODO gps 0対策
static int alt_is_low(Queue* tmp_alt_ring)
{
	int alt_counter = 0;
	int i;
	int qlength = queue_length(tmp_alt_ring)
	for (i = 0; i<qlength; i++)
	{
		double tmp_alt = 0;
		tmp_alt = dequeue(tmp_alt_ring);
		if(tmp_alt<ALT_THRESHOLD) alt_counter++;
	}
	if(alt_counter==qlength)
	{
		printf("ALT IS LOW"\n);
		//高度が全て基準値以下
		return ALT_IS_LOW;
	}
	//高度が基準以上
	printf("ALR IS HIGH\n");
	return ALT_IS_HIGH;
}

//gps三軸安定
static int gps_3_stable()
{
	Queue *lat_ring = make_queue(GPS_FLIGHT_RING);
	Queue *lon_ring = make_queue(GPS_FLIGHT_RING);
	Queue *alt_ring = make_queue(GPS_FLIGHT_RING);
	while(!is_full(lat_ring))
	{
		loc_t flight_gps;
		gps_location(&flight_gps);
		time_t tcurrent;
		time(&tcurrent);
		printf("%s\n",ctime(&tcurrent));
		printf("latitude :%f\nlongitude :%f\naltitude:%f\n",
		       flight_gps.latitude,flight_gps.longitude,flight_gps.altitude);
		enqueue(lat_ring,flight_gps.latitude);
		enqueue(lon_ring,flight_gps.longitude);
		enqueue(alt_ring,flight_gps.altitude);
		sleep(GPS_3AXIS_INTERVAL);
	}
	if(calc_variation(lat_ring)<ABSLAT_THRESHOLD &&
	   calc_variation(lon_ring)<ABSLON_THRESHOLD &&
	   calc_variation(alt_ring)<ABSALT_THRESHOLD &&
	   alt_is_low(alt_ring))
	//TODO ゼロで帰ってくる対策
	{
		printf("GPS3_IS_STABLE\n");
		return GPS3_IS_STABLE;
	}
	sleep(GPS_3AXIS_INTERVAL);
	return GPS3_NOT_STABLE;
}

//gps高度安定
static int gps_alt_stable()
{
	Queue *gpsflight_alt_ring = make_queue(GPS_FLIGHT_RING);
	int i=0;
	while(!is_full(gpsflight_alt_ring))
	{
		loc_t lflight;
		gps_location(&lflight);
		//以下落下中ログデータ、時間緯度経度高度を送る
		time_t tcurrent;
		time(&tcurrent);
		printf("getting gps : %d\n",i++);
		printf("%s\n",ctime(&tcurrent));
		printf("latitude:%f longitude:%f altitude:%f\n",
		       lflight.latitude,lflight.longitude,lflight.altitude);
		//以上ログデータ
		enqueue(gpsflight_alt_ring,lflight.altitude);
		sleep(GPS_ALT_INTERVAL);
	}
	if(calc_variation(gpsflight_alt_ring)<ABSALT_THRESHOLD &&
	   alt_is_low(gpsflight_alt_ring)==ALT_IS_LOW)
	{
		printf("ALT IS STABLE and LOW\n");
		return GPS_ALT_IS_STABLE;
	}
	printf("ALT IS NOT STABLE OR NOT LOW\n");
	sleep(GPS_ALT_INTERVAL);
	return GPS_ALT_NOT_STABLE;
}

//前回のシーケンスの終了時間からの分を返す
int diffmin(Sequence last_seq)
{
	time_t tcurrent;
	time(&tcurrent);
	double delta_min=difftime(tcurrent,last_seq.last_time)/60;//分に変換
	return (int)delta_min;
}

//timeout 判定の関数 引数はタイムアウト時間(分)とSequence構造体
int time_is_out(int timeout_min,Sequence last_seq)
{
	if(diffmin(last_seq) > timeout_min) {
		//前のステータス終了からの経過時間 > タイムアウト時間
		return TIME_IS_OUT;
	}
	else return TIME_NOT_OUT;
}

//シーケンス開始
int start_seq(Sequence *start_seq)
{
	write_sequence(start_seq,START_SEQ);
	printf("FIRST SEQUENCE START\n");
	return 0;
}

//放出判定シーケンス
static int release(Sequence *release_seq)
{
	int light_counter = 0;//islight()return 1した回数
	while(1)
	{
		//放出判定が出るまで繰り返す(timeoutによる抜け出しあり)
		if(light_counter == 9)
		{
			//10回連続でislightが1
			printf("release complete:lux sensor\n");
			write_sequence(release_seq,RELEASE_COMPLETE_SEQ);
			return RELEASE_COMPLETE;
		}
		else if(time_is_out(TIMEOUT_LUX,*release_seq)==TIME_IS_OUT)
		{
			//タイムアウト
			printf("release_complete:time out\n");
			write_sequence(release_seq,RELEASE_TIMEOUT_SEQ);
			return RELEASE_TIMEOUT;
		}
		else if(islight() == 1)
		{
			light_counter++;
		}
		else
		{
			light_counter = 0; //islightで暗い判定 カウンターを0に戻してやり直し
		}
		sleep(2); //2秒間空ける
	}
}

//着地判定タイムアウトせずに成功した場合
static int land_release_suc(Sequence *land_seq)
{
	printf("Release success landing\n");
	while(1)
	{
		//高度安定ダブルチェック
		if(gps_alt_stable()==GPS_ALT_IS_STABLE&&gps_alt_stable()==GPS_ALT_IS_STABLE)
		{
			printf("landing_complete;timeout\n");
			write_sequence(land_seq,LAND_COMPLETE_SEQ);
			return 0;
		}else if(time_is_out(TIMEOUT_ALT_STABLE,*land_seq)==TIME_IS_OUT)
		{
			printf("landing_complete;timeout\n");
			write_sequence(land_seq,LAND_COMPLETE_SEQ);
			return 0;
		}
	}
}

//着地判定タイムアウトした場合の処理(例外処理)
static int land_release_timeout(Sequence *land_seq)
{
	printf("Release timeout landing\n");
	while(1)
	{
		if(gps_3_stable()==GPS3_IS_STABLE)
		{
			printf("landing_complete(judged by 3 axis)\n");
			write_sequence(land_seq,LAND_COMPLETE_SEQ);
			return 0;
		}
		else if(time_is_out(TIMEOUT_3_STABLE,*land_seq)==TIME_IS_OUT)
		{
			printf("landing_complete(timeout 3 axis stable)\n");
			write_sequence(land_seq,LAND_COMPLETE_SEQ);
			return 0;
		}
	}
}

int open_case(Sequence *open_seq)
{
	printf("started open case sequence\n");
	cut_initializer();
	cut();
	write_sequence(open_seq,OPEN_CASE_SEQ);
	return 0;
}

//TODO 電源瞬断対策復旧してシーケンス番号からのswitch-case文
int main(void)
{
	luxsensor_initializer();
	gps_init();
	Sequence sequence;
	start_seq(&sequence);
	if (release(&sequence)==RELEASE_COMPLETE)
	{
		land_release_suc(&sequence);
	}else{
		land_release_timeout(&sequence);
	}
	open_case(&sequence);
}
