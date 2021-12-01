#ifndef __REQUEST_FIFO_H__
#define __REQUEST_FIFO_H__

#include "request.h"

typedef struct {
    int fill, use, size;
    Request buffer[MAXREQ];
} Fifo;

void init_fifo(Fifo *f);

void put(Fifo *f, Request r);

Request get(Fifo *f);

#endif // __REQUEST_FIFO_H__
