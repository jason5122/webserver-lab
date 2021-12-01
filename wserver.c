#include "io_helper.h"
#include "request.h"
#include "request_fifo.h"
#include "request_priority_queue.h"
#include <pthread.h>
#include <stdio.h>

struct fifo fifo;
struct priority_queue pq;

pthread_cond_t arrived = PTHREAD_COND_INITIALIZER;
pthread_cond_t used = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_connection(void *arg) {
    sleep(5);

    while (true) {
        pthread_mutex_lock(&mutex);
        while (pq.size == 0)
            pthread_cond_wait(&arrived, &mutex);
        struct request request = get(&fifo);
        struct request request2 = dequeue(&pq);
        printf("%s size: %lld\n", request2.filename, request2.sbuf.st_size);
        pthread_mutex_unlock(&mutex);

        // sleep(3); // TODO: remove; tests multithreading
        request_handle(&request);

        pthread_mutex_lock(&mutex);
        close_or_die(request.fd);

        pthread_cond_signal(&used);

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    int c;
    char *root_dir = "."; // default root
    int port = 10000;

    while ((c = getopt(argc, argv, "d:p:")) != -1)
        switch (c) {
        case 'd':
            root_dir = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        default:
            fprintf(stderr, "usage: wserver [-d basedir] [-p port]\n");
            exit(1);
        }
    
    chdir_or_die(root_dir);

    init_fifo(&fifo);
    init_priority_queue(&pq);

    pthread_t workers[MAXREQ];
    for (int i = 0; i < MAXREQ; i++)
        pthread_create(&workers[i], NULL, handle_connection, NULL);

    int listen_fd = open_listen_fd_or_die(port);
    while (true) {
        pthread_mutex_lock(&mutex);
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);
        pthread_mutex_unlock(&mutex);

        int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr,
                                    (socklen_t *) &client_len);

        pthread_mutex_lock(&mutex);
        while (pq.size == MAXREQ)
            pthread_cond_wait(&used, &mutex);

        struct request request;
        request_parse(conn_fd, &request);
        put(&fifo, request);
        enqueue(&pq, request);

        pthread_cond_broadcast(&arrived);

        pthread_mutex_unlock(&mutex);
    }

    return 0;
}
