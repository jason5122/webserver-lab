#ifndef __REQUEST_FIFO_H__
#define __REQUEST_FIFO_H__

#include "request.h"

struct fifo {
    int fill, use, size;
    struct request buffer[MAXREQ];
};

void init_fifo(struct fifo *f);

void put(struct fifo *f, struct request r);

struct request get(struct fifo *f);

#endif // __REQUEST_FIFO_H__
