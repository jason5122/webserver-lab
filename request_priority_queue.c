#include "request_priority_queue.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

// void swap(Request *heap, int i, int j) {
//     Request tmp = heap[i];
//     heap[i] = heap[j];
//     heap[j] = tmp;
// }

void init_priority_queue(PriorityQueue *pq) { pq->size = 0; }

void min_heapify(PriorityQueue *pq, int i) {
    int l = LEFT(i);
    int r = RIGHT(i);
    int largest = i;

    if (l < pq->size && pq->heap[l].sbuf.st_size < pq->heap[i].sbuf.st_size)
        largest = l;
    if (r < pq->size &&
        pq->heap[r].sbuf.st_size < pq->heap[largest].sbuf.st_size)
        largest = r;

    if (largest != i) {
        // swap(pq->heap, i, largest);
        Request tmp = pq->heap[i];
        pq->heap[i] = pq->heap[largest];
        pq->heap[largest] = tmp;
        min_heapify(pq, largest);
    }
}

void increase_key(PriorityQueue *pq, int i, Request key) {
    if (key.sbuf.st_size > pq->heap[i].sbuf.st_size) {
        fprintf(stderr, "error: new key is larger than current key");
        exit(1);
    }

    pq->heap[i] = key;
    while (i > 0 &&
           pq->heap[PARENT(i)].sbuf.st_size > pq->heap[i].sbuf.st_size) {
        // swap(pq->heap, i, PARENT(i));
        Request tmp = pq->heap[i];
        pq->heap[i] = pq->heap[PARENT(i)];
        pq->heap[PARENT(i)] = tmp;
        i = PARENT(i);
    }
}

void enqueue(PriorityQueue *pq, Request key) {
    if (pq->size + 1 > MAXSIZE) {
        fprintf(stderr, "error: heap overflow");
        exit(1);
    }

    pq->size += 1;
    pq->heap[pq->size - 1] =
        (Request){-1, -1, 0, (struct stat){.st_size = INT_MAX}};
    increase_key(pq, pq->size - 1, key);
}

Request dequeue(PriorityQueue *pq) {
    if (pq->size < 1) {
        fprintf(stderr, "error: heap underflow");
        exit(1);
    }

    Request min = pq->heap[0];
    pq->heap[0] = pq->heap[pq->size - 1];
    pq->size -= 1;
    min_heapify(pq, 0);
    return min;
}
