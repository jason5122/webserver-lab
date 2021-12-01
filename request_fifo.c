#include "request_fifo.h"

void init_fifo(struct fifo *f) {
    f->fill = 0;
    f->use = 0;
    f->size = 0;
}

void put(struct fifo *f, struct request r) {
    f->buffer[f->fill] = r;
    f->fill = (f->fill + 1) % MAXREQ;
    f->size++;
}

struct request get(struct fifo *f) {
    struct request r = f->buffer[f->use];
    f->use = (f->use + 1) % MAXREQ;
    f->size--;
    return r;
}
