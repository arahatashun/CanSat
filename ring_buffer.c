#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "ring_buffer.h"

/*
   キューの生成
   返り値はqueueポインタ
 */
Queue *make_queue(int n)
{
	Queue *que = malloc(sizeof(Queue));
	if (que != NULL)
	{
		que->front = 0;
		que->rear = 0;
		que->count = 0;
		que->size = n;
		que->buff = malloc(sizeof(double) * n);
		if (que->buff == NULL)
		{
			free(que);
			printf("メモリ不足\n");
			return NULL;//segmantaion fault起こす
		}
	}
	return que;
}

int queue_length(Queue *que)
{
	return que->count;
}

int is_full(Queue *que)
{
	return que->count == que->size;
	//trueならば1を返す
}

//Queueの最後に追加した値を返すだけ
double getLast(Queue *que)
{
	if (is_empty(que))
	{
		printf("queue is empty\n");
		return 0;
	}
	return que->buff[que->rear-1];
}

//queueに追加
int enqueue(Queue *que, double x)
{
	if (que->rear == que->size)
	{
		que->rear = 0;
	}
	if (is_full(que))
	{
		printf("enqueue failed\n");
		return -1; //false
	}
	que->buff[que->rear++] = x;
	que->count++;
	return 0;//true
}

// キューは空か
int is_empty(Queue *que)
{
	return que->count == 0;
}

// データを取り出す
double dequeue(Queue *que)
{
	if (is_empty(que))
	{
		printf("queue is empty\n");
		return 0;
	}
	double x = que->buff[que->front];
	que->front++;
	que->count--;
	if (que->front == que->size)
	{
		que->front = 0;
	}
	return x;
}

//キューを削除
int queue_delete(Queue *que)
{
	free(que->buff);
	free(que);
	return 0;
}

//double型用の比較関数
static int dCmp(const void *p, const void *q )
{
	if( *(double*)p > *(double*)q ) return 1;
	if( *(double*)p < *(double*)q ) return -1;
	return 0;
}

//キューの中身のmaxとminの差を計算
double queue_diff(Queue *que)
{
	int i;
	int n = que->count;
	double list[n];
	for(i=0; i<n; i++)
	{
		if(que->front+i <= que->size-1)
		{
			list[i] = (double)que->buff[que->front+i];
		}
		else
		{
			list[i] = (double)que->buff[que->front+i-que->size];
		}
	}
	qsort(list,n,sizeof(double),dCmp);
	return list[n-1] - list[0];
}
