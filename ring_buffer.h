#ifndef RING_BUFFER
#define RING_BUFFER

//First In First Out
typedef struct {
	int front;
	int rear;
	int count;
	int size;
	double *buff;//queueの本体
} Queue;

Queue *make_queue(int n);

int queue_length(Queue *que);

int is_full(Queue *que);

int enqueue(Queue *que, double x);

double dequeue(Queue *que);

int is_empty(Queue *que);

int queue_delete(Queue *que);

#endif
