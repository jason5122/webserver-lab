#ifndef __REQUEST_ULTRA_H__
#define __REQUEST_ULTRA_H__

#include "request.h"

struct node {
    struct request request;
    struct node *next;
};

void push(struct node **head, struct request request);

void push_ordered(struct node **head, struct request request);

struct request pop(struct node **head);

#endif // __REQUEST_ULTRA_H__
