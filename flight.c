#include <stdio.h>
#include <time.h>
#include <math.h>

//sequence_numの定義について
static const int START_SEQ = 1;
static const int ALT_IS_HIGH = -1;
static const int ALT_IS_LOW = 0;
static const int GPS_FLIGHT_RING = 10;//ring_bufferの長さ

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
int write_sequence(Sequence* sequence2write)
{
	FILE *fp=fopen("sequence.txt","a");//追加書き込み
	if(fp==NULL)
	{
		printf("Cannot open sequence\n");
		return -1;
	}else{
		fprintf(fp,"%d:%ld\n",sequence2write->sequence_num,sequence2write->last_time);
		printf("write sequence %d:%s\n",sequence2write->sequence_num,
		       ctime(&sequence2write->last_time));
		fclose(fp);
	}
	return 0;
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

static int alt_is_low(Queue* tmp_alt_ring)
{
	int alt_counter = 0;
	for (int i=0; i<queue_length(tmp_alt_ring); i++)
	{
		double tmpi = 0;
		tmpi = dequeue(tmp_alt_ring);
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
		gps_location(&)
	}
}

int start_seq(void)
{
	time_t timer;
	time (&timer);
	printf("SEQUENCE START");
	sequence.sequence_num = START_SEQ;
	sequence.last_time = timer;
	write_sequence(&sequence);
	return 0;
}

int main(void)
{
	int i;
	time_t timer;
	time(&timer);
}
