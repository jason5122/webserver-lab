#include "request_priority_queue.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

// void swap(struct request *heap, int i, int j) {
//     struct request tmp = heap[i];
//     heap[i] = heap[j];
//     heap[j] = tmp;
// }

void init_priority_queue(struct priority_queue *pq) { pq->size = 0; }

void min_heapify(struct priority_queue *pq, int i) {
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
        struct request tmp = pq->heap[i];
        pq->heap[i] = pq->heap[largest];
        pq->heap[largest] = tmp;
        min_heapify(pq, largest);
    }
}

void increase_key(struct priority_queue *pq, int i, struct request key) {
    if (key.sbuf.st_size > pq->heap[i].sbuf.st_size) {
        fprintf(stderr, "error: new key is larger than current key");
        exit(1);
    }

    pq->heap[i] = key;
    while (i > 0 &&
           pq->heap[PARENT(i)].sbuf.st_size > pq->heap[i].sbuf.st_size) {
        // swap(pq->heap, i, PARENT(i));
        struct request tmp = pq->heap[i];
        pq->heap[i] = pq->heap[PARENT(i)];
        pq->heap[PARENT(i)] = tmp;
        i = PARENT(i);
    }
}

void enqueue(struct priority_queue *pq, struct request key) {
    if (pq->size + 1 > MAXSIZE) {
        fprintf(stderr, "error: heap overflow");
        exit(1);
    }

    pq->size += 1;
    pq->heap[pq->size - 1] =
        (struct request){-1, -1, 0, (struct stat){.st_size = LLONG_MAX}};
    increase_key(pq, pq->size - 1, key);
}

struct request dequeue(struct priority_queue *pq) {
    if (pq->size < 1) {
        fprintf(stderr, "error: heap underflow");
        exit(1);
    }

    struct request min = pq->heap[0];
    pq->heap[0] = pq->heap[pq->size - 1];
    pq->size -= 1;
    min_heapify(pq, 0);
    return min;
}
