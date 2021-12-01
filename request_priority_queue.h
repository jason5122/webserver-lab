#ifndef __REQUEST_PRIORITY_QUEUE_H__
#define __REQUEST_PRIORITY_QUEUE_H__

#include "request.h"

#define MAXSIZE 4

#define PARENT(i) i / 2
#define LEFT(i) 2 * i
#define RIGHT(i) 2 * i + 1

struct priority_queue {
    int size;
    struct request heap[MAXSIZE];
};

void init_priority_queue(struct priority_queue *pq);

void min_heapify(struct priority_queue *pq, int i);

void increase_key(struct priority_queue *pq, int i, struct request key);

void enqueue(struct priority_queue *pq, struct request key);

struct request dequeue(struct priority_queue *pq);

#endif // __REQUEST_PRIORITY_QUEUE_H__
