#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <stdbool.h>
#include <sys/stat.h>

#define MAXBUF 8192
#define MAXREQ 8

struct request {
    int fd;
    bool is_static, is_not_found, is_forbidden;
    struct stat sbuf;
    char method[MAXBUF], filename[MAXBUF], cgiargs[MAXBUF];
    struct request *next;
};

void request_parse(int fd, struct request *r);

void request_handle(struct request *r);

#endif // __REQUEST_H__
