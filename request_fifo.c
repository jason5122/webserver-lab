#include "request_fifo.h"

void init_fifo(Fifo *f) {
    f->fill = 0;
    f->use = 0;
    f->size = 0;
}

void put(Fifo *f, Request r) {
    f->buffer[f->fill] = r;
    f->fill = (f->fill + 1) % MAXREQ;
    f->size++;
}

Request get(Fifo *f) {
    Request r = f->buffer[f->use];
    f->use = (f->use + 1) % MAXREQ;
    f->size--;
    return r;
}
