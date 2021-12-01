#ifndef __REQUEST_H__

#include <sys/types.h>
#include <sys/stat.h>

#define MAXBUF 8192

typedef struct {
    int fd, is_static, is_not_found;
    struct stat sbuf;
    // char *method, *filename, *cgiargs;
    char method[MAXBUF], filename[MAXBUF], cgiargs[MAXBUF];
} Request;

void request_parse(int fd, Request *r);

void request_handle(Request *r);

off_t request_get_filesize(int fd);

#endif // __REQUEST_H__
