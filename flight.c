#include <stdio.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <gps.h>

#include "luxsensor.h"
#include "gut.h"
#include "ring_buffer.h"
#include "xbee_at.h"

//sequence_numの定義について
static const int START_SEQ = 1;
static const int RELEASE_SEQ = 2; //放出判定
static const int LAND_SEQ = 3;
static const int OPEN_SEQ = 4;//ケーシング展開終了
//タイムアウト時間 (分)
static const int TIMEOUT_LUX = 60; //光センサー放出判定
static const int TIMEOUT_ALT_STABLE = 40; //gps高度着地判定
static const int GPS_FLIGHT_RING = 10;//ring_bufferの長さ
//THRESHOLD
static const float ABSLAT_THRESHOLD = 0.00005; //GPS緯度情報安定判定閾値
static const float ABSLON_THRESHOLD = 0.00005; //GPS経度情報安定判定閾値
static const int ABSALT_THRESHOLD = 5; //GPS高度情報安定判定閾値
static const int ALT_THRESHOLD = 100; //GPS高度情報一定値以下判定閾値(m)
static const int GPS_ALT_INTERVAL = 2; //GPS高度取得間隔(gps_altstable内) second

typedef struct st_Sequence {
	int sequence_num;//前のシーケンス番号
	time_t last_time;//前のシーケンスの時間
	//longと同じ
}Sequence;


//前回のシーケンスの終了時刻と現在のシーケンスを取得
int read_sequence(Sequence*sequence2read)
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
	//FILEが存在しないときは追加書き込み
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
	return (int)delta_min;
}

//timeout 判定の関数 引数はタイムアウト時間(分)とSequence構造体
int isTimeout(int timeout_min,Sequence eq)
{
	if(diffmin(seq) > timeout_min)
	{
		//前のステータス終了からの経過時間 > タイムアウト時間
		return 1;
	}else return 0;
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
	int isLightCount = 0;
	while(!isTimeout(TIMEOUT_LUX,*seq))
	{
		if(isLight())
		{
			isLightCount++;
			printf("light_counter:%d\n",isLightCount);
			xbeePrintf("light_counter:%d\n",isLightCount);
		}
		else
		{
			isLightCount = 0;
		}
		if(isLightCount==10)
		{
			printf("release complete:lux sensor\n");
			xbeePrintf("release complete:lux sensor\n");
			write_sequence(seq,RELEASE_SEQ);
			return 0;
		}
		sleep(2);
	}
	printf("release_complete:time out\n", );
	xbeePrintf("release_complete:time out\n");
	write_sequence(seq,RELEASE_SEQ);
	return 0;
}


//Queueの中の値の変動を返す
//NOTE 内部でdequeue
static double calc_variation(Queue *ring)
{
	double variation= INF;//0にするよりも大きくする方があとでtheresholdと比較するので適切
	double first = dequeue(ring);
	double last = getLast(ring);
	//gps return 0 hanle
	if(first!=0.0&&last!=0.0)
	{
		variation = fabs(first - last);
	}
	printf("VARIATION :%f\n",variation);
	return variation;
}

//ring_bufferを受け取り高度が閾値より低いか判定
static int isAltlow(Queue* ring)
{
	double alt = getLast(ring);
	if(alt!=0.0&&alt<ALT_THRESHOLD)
	{
		printf("ALT IS LOW\n");
		xbeePrintf("ALT IS LOW\n")
		return 1;
	}
	//高度が基準以上
	printf("ALT IS HIGH\n");
	xbeePrintf("ALT IS HIGH\n")
	return 0;
}

//着地判定
static int isLanded(queue* ring)
{
	while(!is_full(ring))
	{
		loc_t lflight;
		gps_location(&lflight);
		time_t tcurrent;
		time(&tcurrent);
		printf("%s\n",ctime(&tcurrent));
		printf("latitude:%f longitude:%f altitude:%f\n",
		       lflight.latitude,lflight.longitude,lflight.altitude);
		xbeePrintf("latitude:%f longitude:%f altitude:%f\n",
		           lflight.latitude,lflight.longitude,lflight.altitude);
		enqueue(ring,lflight.altitude);
		sleep(GPS_ALT_INTERVAL);
	}
	if(calc_variation(ring)<ABSALT_THRESHOLD&&isAltlow(ring))
	{
		printf("ALT IS STABLE and LOW\n");
		xbeePrintf("ALT IS STABLE and LOW\n");
		return 1;
	}
	printf("ALT IS NOT STABLE OR NOT LOW\n");
	xbeePrintf("ALT IS NOT STABLE OR NOT LOW\n");
	return 0;
}


static int landSeq(Sequence* seq)
{
	printf("Landing Sequence\n");
	Queue *ring = make_queue(GPS_FLIGHT_RING);
	while(!isTimeout(TIMEOUT_ALT_STABLE,*seq))
	{
		if(isLanded(ring))
		{
			printf("landing_complete:landed\n");
			xbeePrintf("landing_complete:landed\n");
			write_sequence(land_seq, LAND_SEQ);
			queue_delete(ring);
			return 0;
		}
	}
	queue_delete(ring);
	printf("landing_complete;timeout\n");
	write_sequence(land_seq,LAND_COMPLETE_SEQ);
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

//TODO 電源瞬断対策復旧してシーケンス番号からのswitch-case文
int main(void)
{
	xbee_init();
	luxsensor_initialize();
	gps_init();
	Sequence sequence;
	if(read_sequence(&sequence)!=0)
	//file open に失敗
	{
		start_seq(&sequence);
		isReleased(&sequence);
		luxsensor_close();
		isLanded(&sequence);
		open_case(&sequence);
		return 0;
	}
	switch (sequence.sequence_num)
	{
	case 1:
		isReleased(&sequence);
		luxsensor_close();
	case 2:
		isLanded(&sequence);
	case 3:
		open_case(&seq);
	case 4:
		break;
	}
	return 0;
}
