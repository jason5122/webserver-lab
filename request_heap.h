#ifndef __REQUEST_HEAP_H__

typedef struct {
    int size;
    int max_size;
    int *heap;
} MinHeap;

MinHeap *init_heap(int max_size);

void insert(MinHeap *h, Request key);

Request extract_min(MinHeap *h);

#endif // __REQUEST_HEAP_H__
