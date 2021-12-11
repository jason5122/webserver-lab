#include "io_helper.h"
#include <pthread.h>

#define MAXBUF 8192

// Send an HTTP request for the specified file
void *client_send(int fd, char *filename) {
    char buf[MAXBUF];
    char hostname[MAXBUF];

    gethostname_or_die(hostname, MAXBUF);

    /* Form and send the HTTP request */
    sprintf(buf, "GET %s HTTP/1.1\n", filename);
    sprintf(buf, "%shost: %s\n\r\n", buf, hostname);
    write_or_die(fd, buf, strlen(buf));
    return NULL;
}

// Read the HTTP response and print it out
void *client_print(int fd) {
    char buf[MAXBUF];
    int n;

    // Read and display the HTTP Header
    n = readline_or_die(fd, buf, MAXBUF);
    while (strcmp(buf, "\r\n") && (n > 0)) {
        printf("Header: %s", buf);
        n = readline_or_die(fd, buf, MAXBUF);
    }

    // Read and display the HTTP Body
    n = readline_or_die(fd, buf, MAXBUF);
    while (n > 0) {
        printf("%s", buf);
        n = readline_or_die(fd, buf, MAXBUF);
    }
    return NULL;
}

typedef struct __client_t {
    char *host, *filename;
    int port;
    int state;
} client_t;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int state = 0;

void *make_request(void *arg) {
    pthread_mutex_lock(&mutex);
    client_t *r = (client_t *) arg;
    while (state < r->state)
        pthread_cond_wait(&cond, &mutex);

    // printf("%d: opening connection to %s:%d...\n", r->state, r->host,
    // r->port);

    int clientfd = open_client_fd_or_die(r->host, r->port);
    // printf("%d, %s: sending info...\n", r->state, r->filename);

    client_send(clientfd, r->filename);
    state++;
    pthread_cond_broadcast(&cond);

    pthread_mutex_unlock(&mutex);
    client_print(clientfd);
    pthread_mutex_lock(&mutex);

    close_or_die(clientfd);

    // printf("%d, %s: closing connection\n\n", r->state, r->filename);

    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <host> <port> <filename>...\n", argv[0]);
        exit(1);
    }

    const int THREADS = argc - 3;
    pthread_t threads[THREADS];
    client_t requests[THREADS];

    for (int i = 0; i < THREADS; i++) {
        requests[i] = (client_t){argv[1], argv[i + 3], atoi(argv[2]), i};
        pthread_create(&threads[i], NULL, make_request, &requests[i]);
    }

    for (int i = 0; i < THREADS; i++)
        pthread_join(threads[i], NULL);

    exit(0);
}
