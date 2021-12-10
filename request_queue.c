#include "request_queue.h"
#include <stdio.h>
#include <stdlib.h>

void push(struct node **head, struct request request) {
    // linked list is empty
    if (*head == NULL) {
        *head = malloc(sizeof(struct node));
        (*head)->request = request;
        (*head)->next = NULL;
        return;
    }

    struct node *curr = *head;
    while (curr->next)
        curr = curr->next;

    curr->next = malloc(sizeof(struct node));
    curr->next->request = request;
    curr->next->next = NULL;
}

void push_ordered(struct node **head, struct request request) {
    // linked list is empty
    if (*head == NULL) {
        *head = malloc(sizeof(struct node));
        (*head)->request = request;
        (*head)->next = NULL;
        return;
    }
    // insert is smaller than head
    else if (request.sbuf.st_size < (*head)->request.sbuf.st_size) {
        struct node *temp = *head;
        *head = malloc(sizeof(struct node));
        (*head)->request = request;
        (*head)->next = temp;
        return;
    }

    struct node *curr = *head;
    while (curr->next &&
           request.sbuf.st_size > curr->next->request.sbuf.st_size)
        curr = curr->next;

    struct node *temp = curr->next;
    curr->next = malloc(sizeof(struct node));
    curr->next->request = request;
    curr->next->next = temp;
}

struct request pop(struct node **head) {
    if (*head == NULL) {
        fprintf(stderr, "error: linked list underflow");
        exit(1);
    }

    struct node *next = (*head)->next;
    struct request request = (*head)->request;
    free(*head);
    *head = next;

    return request;
}
