#ifndef __REQUEST_PRIORITY_QUEUE_H__
#define __REQUEST_PRIORITY_QUEUE_H__

#include "request.h"

#define MAXSIZE 4

#define PARENT(i) i / 2
#define LEFT(i) 2 * i
#define RIGHT(i) 2 * i + 1

typedef struct {
    int size;
    Request heap[MAXSIZE];
} PriorityQueue;

void init_priority_queue(PriorityQueue *pq);

void min_heapify(PriorityQueue *pq, int i);

void increase_key(PriorityQueue *pq, int i, Request key);

void enqueue(PriorityQueue *pq, Request key);

Request dequeue(PriorityQueue *pq);

#endif // __REQUEST_PRIORITY_QUEUE_H__
