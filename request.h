#ifndef __REQUEST_H__

#include <sys/types.h>
#include <sys/stat.h>

typedef struct {
    int fd, is_static, is_not_found;
    struct stat sbuf;
    char *method, *filename, *cgiargs;
} Request;

Request request_parse(int fd);

void request_handle(Request *r);

off_t request_get_filesize(int fd);

#endif // __REQUEST_H__
