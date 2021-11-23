#include "io_helper.h"
#include "request.h"
#include <pthread.h>
#include <stdio.h>

#define THREADS 16
// #define THREADS 126 // hard limit??

int buffer[THREADS];
int fill = 0;
int use = 0;
int count = 0;

void put(int value) {
    buffer[fill] = value;
    fill = (fill + 1) % THREADS;
    count++;
}

int get() {
    int tmp = buffer[use];
    use = (use + 1) % THREADS;
    count--;
    return tmp;
}

typedef struct {
    int id; // TODO: remove; debug use
} Connection;

pthread_cond_t arrived = PTHREAD_COND_INITIALIZER;
pthread_cond_t used = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_connection(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        Connection *c = (Connection *) arg;
        printf("%d: starting worker (count = %d)...\n", c->id, count);
        while (count == 0) {
            pthread_cond_wait(&arrived, &mutex);
            printf("%d: awoke with count %d...\n", c->id, count);
        }
        int conn_fd = get();
        pthread_mutex_unlock(&mutex);

        printf("%d: sleeping...\n", c->id);
        sleep(3); // TODO: remove; tests multithreading

        pthread_mutex_lock(&mutex);
        request_handle(conn_fd);
        close_or_die(conn_fd);

        pthread_cond_signal(&used);

        printf("%d: finishing worker (count = %d)...\n", c->id, count);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *connect_thread(void *arg) {
    int port = *((int *) arg);
    int listen_fd = open_listen_fd_or_die(port);
    while (1) {
        pthread_mutex_lock(&mutex);
        printf("starting manager...\n");
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);
        pthread_mutex_unlock(&mutex);

        int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr,
                                    (socklen_t *) &client_len);

        pthread_mutex_lock(&mutex);
        while (count == THREADS)
            pthread_cond_wait(&used, &mutex);
        put(conn_fd);
        pthread_cond_broadcast(&arrived);

        printf("finishing manager (count = %d)...\n", count);
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

    // run out of this directory
    chdir_or_die(root_dir);
    
    pthread_t manager;
    pthread_create(&manager, NULL, connect_thread, &port);
    
    pthread_t workers[THREADS];
    Connection conns[THREADS];

    for (int i = 0; i < THREADS; i++) {
        conns[i] = (Connection){i};
        pthread_create(&workers[i], NULL, handle_connection, &conns[i]);
    }

    for (int i = 0; i < THREADS; i++)
        pthread_join(workers[i], NULL);
    
    return 0;
}
