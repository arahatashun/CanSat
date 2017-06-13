#include <stdio.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

//sequence_numの定義について
static const int START_SEQ = 1;
static const int RELEASE_COMPLETE_SEQ = 2; //放出判定正常終了
static const int RELEASE_TIMEOUT_SEQ = 5; //放出判定時間切れ
static const int CASING_OPEN_SEQ = 4;//ケーシング展開終了

static const int TIMEOUT_LUX = 60; //光センサー放出判定タイムアウト時間(min)


static const int ALT_IS_HIGH = 1;
static const int ALT_IS_LOW = 0;
static const int GPS3_IS_STABLE = 1;
static const int GPS3_NOT_STABLE = 0;
static const int GPS_ALT_IS_STABLE = 1;
static const int GPS_ALT_NOT_STABLE = 0;
static const int GPS_FLIGHT_RING = 10;//ring_bufferの長さ
static const int TIME_IS_OUT = 1;
static const int TIME_NOT_OUT= 0;
static const int RELEASE_COMPLETE = 0;
static const int RELEASE_TIMEOUT = 1;
static const float ABSLAT_THRESHOLD = 0.00003; //GPS緯度情報安定判定閾値
static const float ABSLON_THRESHOLD = 0.00003; //GPS経度情報安定判定閾値
static const int ABSALT_THRESHOLD = 3; //GPS高度情報安定判定閾値
static const int ALT_THRESHOLD = 100; //GPS高度情報一定値以下判定閾値(m)
static const int GPS_3AXIS_INTERVAL = 2; //GPS高度取得間隔(gps_3axisstable内) second
static const int GPS_ALT_INTERVAL = 2; //GPS高度取得間隔(gps_altstable内) second

typedef struct st_Sequence {
	int sequence_num;//シーケンス番号
	time_t last_time;//一段階前のシーケンスの時間
}Sequence;

Sequence sequence; //global


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
	sequence2write->last_time = tcurrent;
	sequence2write->sequence_num = seq_num2write:
	                               FILE *fp=fopen("sequence.txt","a");//追加書き込み
	if(fp==NULL)
	{
		printf("Cannot open sequence\n");
		return -1;
	}else{
		fprintf(fp,"%d:%ld\n",seq_num2write,tcurrent);
		printf("write sequence %d:%s\n",seq_num2write,
		       ctime(&tcurrent));
		fclose(fp);
	}
	return 0;
}

//Sequence構造体が0になってた時に自動的に直す関数
int update_seq(Sequence *sequence2update)
{

}
//Queueの中の値の変動を返す
static double calc_variation(Queue *gpsflight_tmp_ring)
{
	double INF = 10000;
	double min = INF;
	double max = 0;
	for(int i=0; i<queue_length(gpsflight_tmp_ring); i++)
	{
		double tmpi=0;
		tmpi=dequeue(gpsflight_tmp_ring);
		if(tmpi<min) min = tmpi;
		if(tmpi>max) max = tmpi;
	}
	double abstmp = INF;//0にするよりも大きくする方があとでtheresholdと比較するので適切
	abstmp = fabs(max - min);
	return abstmp;
}

/*
   照度センサータイムアウトの場合は、より慎重に着地を判定するために3軸安定を使い、放出判定が成功してる場合は、
   高速化のためと着地後風で飛ばされることも考慮して高度情報の安定化のみを判定基準にしてる
 */

static int alt_is_low(Queue* tmp_alt_ring)
{
	int alt_counter = 0;
	for (int i=0; i<queue_length(tmp_alt_ring); i++)
	{
		double tmp_alt = 0;
		tmp_alt = dequeue(tmp_alt_ring);
		if(tmpi<ALT_THRESHOLD) alt_counter++;
	}
	if(alt_counter==queue_length(tmp_alt_ring))
	{
		//高度が全て基準値以下
		return ALT_IS_LOW;
	}
	//高度が基準以上
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
	   alt_low(alt_ring))      //0で帰ってくる対策も後々すべき
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
	Queue *gpsflight_alt_ring = make_queue(GPS_RING_LEN);
	while(!is_full(gpsflight_alt_ring))
	{
		loc_t lflight;
		gps_location(&lflight);
		//以下落下中ログデータ、時間緯度経度高度を送る
		time_t tcurrent;
		printf("%s\n",ctime(&tcurrent));
		printf("latitude:%f longitude:%f altitude:%f\n",
		       lflight.latitude,lflight.longitude,lflight.altitude);
		//以上ログデータ
		enqueue(gpsflight_alt_ring,lflight.altitude);
		sleep(GPS_ALT_INTERVAL);
	}
	if(calc_variation(gpsflight_alt_ring)<ABSALT_THRESHOLD &&
	   alt_low(gpsflight_alt_ring))
	{
		return GPS_ALT_IS_STABLE;
	}
	sleep(GPS_ALT_INTERVAL);
	return GPS_ALT_NOT_STABLE;
}

//前回のシーケンスの終了時間からの分を返す
int diffmin(Sequence last_seq)
{
	time_t tcurrent;
	time(&tcurrent);
	double delta_min=difftime(tcurrent-last_seq.last_time)/60;//分に変換
	return (int)delta_min;
}

/*
   timeoutしているかどうかを判定するための関数
   最初はSequence構造体から時刻を読みとり、もしraspberrypiが落ちた落ちて変数がreset
   されてしまっていたら、sequence.txtから前シーケンス終了時刻を読み取る
   引数はtimeout設定分
 */

int time_is_out(int timeout_min,Sequence last_seq){
	if(last_seq.last_time==0)
	{
		//slast.last_time==0->変数がresetされてる ->まずは構造体データを復旧
		read_sequence(&last_seq);
	}
	//次にタイムアウト判定
	if(diffmin(last_seq) > timeout_min) {
		//前のステータス終了からの経過時間 > タイムアウト時間
		return TIME_IS_OUT;
	}
	else return TIME_NOT_OUT;
}


int start_seq(Sequence *start_seq)
{
	time_t timer;
	time (&timer);
	write_sequence(start_seq,START_SEQ);
	printf("FIRST SEQUENCE START");
	return 0;
}

int release(Sequence *release_seq)
{
	int islight_counter = 0;//islight()return 1した回数
	int release_complete = 0;//1になると放出判定フラグ
	while(release_complete!=1)
	{
		//放出判定が出るまで繰り返す(timeoutによる抜け出しあり)
		if(islight_counter == 9)
		{
			//10回連続でislightが1
			release_complete = 1;
			printf("release complete\n");
			write_sequence(release_seq,RELEASE_COMPLETE_SEQ);
			return RELEASE_COMPLETE;
		}
		else if(time_is_out(TIMEOUT_LUX)==TIME_IS_OUT)
		{
			//タイムアウト
			printf("TIMEOUT_LUX;release_complete\n");
			write_sequence(release_seq,RELEASE_TIMEOUT_SEQ);
			return RELEASE_TIMEOUT;
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
	return -1;
}

/*
放出判定が時間切れした場合の処理 sequence構造体の読み込みで判断
*/
static int landing_timeout_ver(){
	printf("enter timeout_ver");
	while(!landing_complete) {
		if(is_timeout(TIMEOUT_GPSSTABLE)) {
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
int open_case(Sequence *open_seq)
{
	printf("started casing_open sequence\n");
	cut_initializer();
	cut();
	time_t tcurrent;
	open_seq->last_time = tcurrent;
	open_seq->sequence_num = CASING_OPEN_SEQ;
	write_sequence(open_seq);
	printf("%s\n",ctime(&tcurrent));
	printf("casing_open\n");
	return 0;
}

int main(void)
{
	int i;
	time_t timer;
	time(&timer);
}
