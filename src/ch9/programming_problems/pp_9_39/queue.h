#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef struct _queue_t {
    int headi;
    int taili;
    int *queue_arr;
    int arr_len;
    int num_entries;
} queue_t;


int alloc_queue(queue_t *q, int qlen);

int free_queue(queue_t *q);

int queue_is_full(queue_t *q);

int enq(queue_t *q, int e);

int queue_is_empty(queue_t *q);

int deq(queue_t *q, int *e);

int queue_contains(queue_t *q, int e);

void test_queue(void);

#endif