//
// client.c: A very, very primitive HTTP client.
//
// To run, try:
//      client hostname portnumber filename
//
// Sends one HTTP request to the specified HTTP server.
// Prints out the HTTP response.
//
// For testing your server, you will want to modify this client.
// For example:
// You may want to make this multi-threaded so that you can
// send many requests simultaneously to the server.
//
// You may also want to be able to request different URIs;
// you may want to get more URIs from the command line
// or read the list from a file.
//
// When we test your server, we will be using modifications to this client.
//

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

        // If you want to look for certain HTTP tags...
        // int length = 0;
        // if (sscanf(buf, "Content-Length: %d ", &length) == 1) {
        //    printf("Length = %d\n", length);
        //}
    }

    // Read and display the HTTP Body
    n = readline_or_die(fd, buf, MAXBUF);
    while (n > 0) {
        printf("%s", buf);
        n = readline_or_die(fd, buf, MAXBUF);
    }
    return NULL;
}

typedef struct {
    char *host, *filename;
    int port;
    int id; // TODO: remove; debug use
} ClientRequest;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *make_request(void *arg) {
    pthread_mutex_lock(&mutex);
    ClientRequest *r = (ClientRequest *) arg;

    printf("%d: opening connection to %s:%d...\n", r->id, r->host, r->port);
    pthread_mutex_unlock(&mutex);

    /* Open a single connection to the specified host and port */
    int clientfd = open_client_fd_or_die(r->host, r->port);

    printf("%d: sending info...\n", r->id);
    client_send(clientfd, r->filename);

    pthread_mutex_lock(&mutex);
    client_print(clientfd);
    close_or_die(clientfd);

    printf("%d: closing connection\n", r->id);

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
    ClientRequest requests[THREADS];

    for (int i = 0; i < THREADS; i++) {
        requests[i] = (ClientRequest){argv[1], argv[i + 3], atoi(argv[2]), i};
        pthread_create(&threads[i], NULL, make_request, &requests[i]);
    }

    for (int i = 0; i < THREADS; i++)
        pthread_join(threads[i], NULL);

    exit(0);
}
