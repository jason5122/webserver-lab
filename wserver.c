#include "io_helper.h"
#include "request.h"
#include "request_heap.h"
#include <pthread.h>
#include <stdio.h>

#define THREADS 16
// #define THREADS 126 // hard limit??

MinHeap *heap;
Request buffer[THREADS];
int fill = 0;
int use = 0;
int count = 0;

void put(Request r) {
    buffer[fill] = r;
    fill = (fill + 1) % THREADS;
    count++;
}

Request get() {
    Request r = buffer[use];
    use = (use + 1) % THREADS;
    count--;
    return r;
}

pthread_cond_t arrived = PTHREAD_COND_INITIALIZER;
pthread_cond_t used = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_connection(void *arg) {
    sleep(5);
    
    while (1) {
        pthread_mutex_lock(&mutex);
        while (count == 0)
            pthread_cond_wait(&arrived, &mutex);
        Request request = get();
        Request test_request = extract_min(heap);
        printf("%s size: %lld\n", test_request.filename,
               test_request.sbuf.st_size);
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

void *connect_thread(void *arg) {
    int port = *((int *) arg);
    int listen_fd = open_listen_fd_or_die(port);
    while (1) {
        pthread_mutex_lock(&mutex);
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);
        pthread_mutex_unlock(&mutex);

        int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr,
                                    (socklen_t *) &client_len);

        pthread_mutex_lock(&mutex);
        while (count == THREADS)
            pthread_cond_wait(&used, &mutex);

        Request request = request_parse(conn_fd);
        put(request);
        insert(heap, request);

        // printf("%s size: %lld\n", request.filename, request.sbuf.st_size);
        pthread_cond_broadcast(&arrived);

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

    heap = init_heap(THREADS);

    pthread_t manager;
    pthread_create(&manager, NULL, connect_thread, &port);

    pthread_t workers[THREADS];
    for (int i = 0; i < THREADS; i++)
        pthread_create(&workers[i], NULL, handle_connection, NULL);

    for (int i = 0; i < THREADS; i++)
        pthread_join(workers[i], NULL);

    return 0;
}
