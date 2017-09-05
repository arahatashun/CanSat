#include <stdio.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <gps.h>
#include "bme280.h"
#include "acclgyro.h"
#include "luxsensor.h"
#include "gut.h"
#include "ring_buffer.h"
#include "xbee_at.h"

//sequence_numの定義について
static const int START_SEQ = 1;
static const int RELEASE_SEQ = 2; //放出判定
static const int LAND_WAIT_SEQ = 3;
static const int LAND_SEQ = 4;
static const int OPEN_SEQ = 5;//ケーシング展開終了

//タイムアウト時間 (分)
static const int TIMEOUT_LUX = 60; //光センサー放出判定
static const int WAIT4LAND_MINS = 10;//NOTE 終端速度に依存
static const int TIMEOUT_ALT_STABLE = 40; //高度着地判定
static const int ALTUTUDE_RING_LEN = 10;//ring_bufferの長さ
//THRESHOLD
static const int ALT_CHANGE_THRESHOLD = 1.5; //高度情報安定判定閾値
static const int MINIMUM_ALTITUDE = 100; //高度情報一定値以下判定閾値(m)
static const int CONTINUOUS_ISLIGHT_TIME = 5;
//センサーデータ取得感覚
static const int LIGHT_INTERVAL = 1;
static const int ALT_INTERVAL_SECONDS = 10;//seconds

static const int WAIT4START_SECONDS = 10;
static const double INF = 10000;


typedef struct st_Sequence
{
	int sequence_num;//前のシーケンス番号
	time_t last_time;//前のシーケンスの時間
	//longと同じ
}Sequence;

//GPS座標を取得して送信
int getGPScoords(void)
{
	gps_flush();
	loc_t coord;
	gps_location(&coord);
	printf("latitude:%f longitude:%f altitude:%f\n",
	       coord.latitude,coord.longitude,coord.altitude);
	xbeePrintf("latitude:%f longitude:%f altitude:%f\n",
	           coord.latitude,coord.longitude,coord.altitude);
	return 0;
}

//高度を取得して送信
double getAltitude(void)
{
	double altitude = readAltitude();
	printf("ALTITUDE:%f\n",altitude);
	xbeePrintf("ALTITUDE:%f\n",altitude);
	return altitude;
}

//前回のシーケンスの終了時刻と現在のシーケンスを取得
int read_sequence(Sequence* sequence2read)
{
	FILE *fp = fopen("sequence.txt","r");
	if(fp==NULL)
	{
		//FILEは存在しないとき
		printf("Cannot open sequence.txt\n");
		xbeePrintf("Cannot opem sequence txt\n");
		return -1;
	}
	else
	{
		while(fscanf(fp,"%d:%ld\n",&(sequence2read->sequence_num),&(sequence2read->last_time))!= EOF)
		{
			;//pass
		}
		printf("read_sequence %d:%ld\n",sequence2read->sequence_num,sequence2read->last_time);
		xbeePrintf("read_sequence %d:%ld\n",sequence2read->sequence_num,sequence2read->last_time);
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
	xbeePrintf("write sequence %d:%s\n",seq_num2write,ctime(&tcurrent));
	FILE *fp=fopen("sequence.txt","a");//追加書き込み
	//FILEが存在しないときは新規作成
	if(fp==NULL)
	{
		printf("Cannot open sequence\n");
		xbeePrintf("Cannot open sequence\n");
		return -1;
	}else
	{
		fprintf(fp,"%d:%ld\n",seq_num2write,tcurrent);
		printf("write sequence success\n");
		xbeePrintf("write sequence success\n");
		fclose(fp);
	}
	return 0;
}

//前回のシーケンスの終了時間からの分を返す
int diffmin(Sequence last_seq)
{
	time_t tcurrent;
	time(&tcurrent);
	double delta_min=difftime(tcurrent,last_seq.last_time)/60;//分に変換
	printf("diffmin:%f\n",delta_min);
	xbeePrintf("diffmin:%f\n",delta_min);
	return (int)delta_min;
}

//timeout 判定の関数 引数はタイムアウト時間(分)とSequence構造体
int isTimeout(int timeout_min,Sequence seq)
{
	if(diffmin(seq) > timeout_min)
	{
		//前のステータス終了からの経過時間 > タイムアウト時間
		return 1;
	}
	return 0;
}

static int wait4Start()
{
	int i=0;
	for(i=0; i<WAIT4START_SECONDS; i++)
	{
		printf("%d seconds to start\n",WAIT4START_SECONDS-i);
		xbeePrintf("%d seconds to start\n",WAIT4START_SECONDS-i);
		sleep(1);
	}
	return 0;
}

//シーケンス開始
static int startSeq(Sequence *seq)
{
	write_sequence(seq,START_SEQ);
	printf("FIRST SEQUENCE START\n");
	return 0;
}

//放出判定シーケンス
static int releaseSeq(Sequence *seq)
{
	printf("RELEASE SEQUENCE START");
	int isLightCount = 0;
	while(!isTimeout(TIMEOUT_LUX,*seq))
	{
		getGPScoords();
		getAltitude();
		while(isLightCount<CONTINUOUS_ISLIGHT_TIME)
		{
			if(isLight())
			{
				isLightCount++;
				printf("light_counter:%d\n",isLightCount);
				xbeePrintf("light_counter:%d\n",isLightCount);
			}else{
				printf("isLight False\n");
				xbeePrintf("isLight False\n");
				isLightCount = 0;
			}
			sleep(LIGHT_INTERVAL);
		}
		printf("release complete:lux sensor\n");
		xbeePrintf("release complete:lux sensor\n");
		write_sequence(seq,RELEASE_SEQ);
		return 0;
	}
	printf("release_complete:time out\n");
	xbeePrintf("release_complete:time out\n");
	write_sequence(seq,RELEASE_SEQ);
	return 0;
}

static int wait4Land(Sequence* seq)
{
	xbeePrintf("LAND WAIT SEQUENCE START\n");
	printf("LAND WAIT SEQUENCE START\n");
	while(!isTimeout(WAIT4LAND_MINS,*seq))
	{
		getGPScoords();
		getAltitude();
	}
	xbeePrintf("LAND WAIT SEQUENCE FINISHED\n");
	printf("LAND WAIT SEQUENCE FINISHED\n");
	write_sequence(seq,LAND_WAIT_SEQ);
	return 0;
}

//Queueの中の値の変動を返す
//NOTE 内部でdequeue
static double calc_variation(Queue *ring)
{
	double variation= INF;//0にするよりも大きくする方があとでtheresholdと比較するので適切
	dequeue(ring);
	variation = queue_diff(ring);
	printf("VARIATION :%f\n",variation);
	return variation;
}

//ring_bufferを受け取り高度が閾値より低いか判定
static int isAltlow(Queue* ring)
{
	double alt = getLast(ring);
	if(alt<MINIMUM_ALTITUDE)
	{
		printf("ALT IS LOW\n");
		xbeePrintf("ALT IS LOW\n");
		return 1;
	}
	//高度が基準以上
	printf("ALT IS HIGH\n");
	xbeePrintf("ALT IS HIGH\n");
	return 0;
}

//着地判定ロジック
static int isLanded(Queue* ring)
{
	while(!is_full(ring))
	{
		getGPScoords();
		time_t tcurrent;
		time(&tcurrent);
		printf("%s\n",ctime(&tcurrent));
		double altitude = getAltitude();
		enqueue(ring,altitude);
		sleep(ALT_INTERVAL_SECONDS);
	}
	if(calc_variation(ring)<ALT_CHANGE_THRESHOLD&&isAltlow(ring))
	{
		printf("ALT IS STABLE and LOW\n");
		xbeePrintf("ALT IS STABLE and LOW\n");
		return 1;
	}
	printf("ALT IS NOT STABLE OR NOT LOW\n");
	xbeePrintf("ALT IS NOT STABLE OR NOT LOW\n");
	return 0;
}

//着地判定シーケンス
static int landSeq(Sequence* seq)
{
	printf("Landing Sequence\n");
	Queue *ring = make_queue(ALTUTUDE_RING_LEN);
	while(!isTimeout(TIMEOUT_ALT_STABLE,*seq))
	{
		if(isLanded(ring))
		{
			printf("landing_complete:landed\n");
			xbeePrintf("landing_complete:landed\n");
			write_sequence(seq, LAND_SEQ);
			queue_delete(ring);
			return 0;
		}
	}
	queue_delete(ring);
	printf("landing_complete;timeout\n");
	write_sequence(seq,LAND_SEQ);
	return 0;
}

int open_case(Sequence *seq)
{
	printf("started open case sequence\n");
	cut_initialize();
	cut_all();
	write_sequence(seq,OPEN_SEQ);
	return 0;
}


int main(void)
{

	xbee_init();
	bme280_initialize();
	acclGyro_initialize();
	luxsensor_initialize();
	gps_init();
	Sequence sequence;

	if(read_sequence(&sequence)!=0)
	//file open に失敗
	{
		wait4Start();
		startSeq(&sequence);
		releaseSeq(&sequence);
		luxsensor_close();
		wait4Land(&sequence);
		landSeq(&sequence);
		open_case(&sequence);
		gps_off();
		return 0;
	}
	switch (sequence.sequence_num)
	{
	case 1:
		releaseSeq(&sequence);
		luxsensor_close();
	case 2:
		wait4Land(&sequence);
	case 3:
		landSeq(&sequence);
	case 4:
		open_case(&sequence);
	case 5:
		gps_off();
		break;
	default:
		printf("Sequence number is Strange\n");
		break;
	}
	return 0;
}
