#include "request.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define PARENT(i) i / 2
#define LEFT(i) 2 * i
#define RIGHT(i) 2 * i + 1

typedef struct {
    int size;
    int max_size;
    Request *heap;
} MinHeap;

void swap(MinHeap *h, int i, int j) {
    Request tmp = h->heap[i];
    h->heap[i] = h->heap[j];
    h->heap[j] = tmp;
}

off_t get_filesize(MinHeap *h, int i) {
    return h->heap[i].is_not_found ? 0 : h->heap[i].sbuf.st_size;
}

off_t get_filesize2(Request r) { return r.is_not_found ? 0 : r.sbuf.st_size; }

MinHeap *init_heap(int max_size) {
    MinHeap *h = (MinHeap *) malloc(sizeof(MinHeap));

    if (h == NULL) {
        fprintf(stderr, "error: could not initialize heap\n");
        exit(1);
    }

    h->size = 0;
    h->max_size = max_size;
    h->heap = (Request *) malloc(max_size * sizeof(Request));

    if (h->heap == NULL) {
        fprintf(stderr, "error: could not initialize heap\n");
        exit(1);
    }

    return h;
}

void minHeapify(MinHeap *h, int i) {
    int l = LEFT(i);
    int r = RIGHT(i);
    int largest = i;

    if (l < h->size && get_filesize(h, l) < get_filesize(h, i))
        largest = l;
    if (r < h->size && get_filesize(h, r) < get_filesize(h, largest))
        largest = r;

    if (largest != i) {
        swap(h, i, largest);
        minHeapify(h, largest);
    }
}

void increaseKey(MinHeap *h, int i, Request key) {
    if (get_filesize2(key) > get_filesize(h, i)) {
        fprintf(stderr, "error: new key is larger than current key\n");
        printf("%lld > %lld\n", get_filesize2(key), get_filesize(h, i));
        exit(1);
    }

    h->heap[i] = key;
    while (i > 0 && get_filesize(h, PARENT(i)) > get_filesize(h, i)) {
        swap(h, i, PARENT(i));
        i = PARENT(i);
    }
}

void insert(MinHeap *h, Request key) {
    if (h->size + 1 > h->max_size) {
        fprintf(stderr, "error: heap overflow\n");
        exit(1);
    }

    h->size += 1;
    h->heap[h->size - 1] =
        (Request){-1, -1, 0, (struct stat){.st_size = INT_MAX}};
    increaseKey(h, h->size - 1, key);
}

Request get_min(MinHeap *h) { return h->heap[0]; }

Request extract_min(MinHeap *h) {
    if (h->size < 1) {
        fprintf(stderr, "error: heap underflow\n");
        exit(1);
    }

    Request min = get_min(h);
    h->heap[0] = h->heap[h->size - 1];
    h->size -= 1;
    minHeapify(h, 0);
    return min;
}
