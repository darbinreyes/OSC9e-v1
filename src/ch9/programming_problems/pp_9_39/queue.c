/*!
    @header Array based queue
    An interface to array based queue.
*/
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*

typedef struct _queue_t {
    int headi;
    int taili;
    int *queue_arr;
    int arr_len;
    int num_entries;
} queue_t;

*/

/*!
    @function alloc_queue

    @discussion Initializes a queue supporting a maximum of qlen entries.

    @param q    Pointer to a queue_t

    @param qlen    Desired max. number of queue entries.

    @result 0 on success. Other values indicate error.
*/
int alloc_queue(queue_t *q, int qlen) {

    if (q == NULL || qlen <= 0) {
        assert(0);
        return 1;
    }

    q->headi = 0;
    q->taili = 0;
    /*
        qlen + 1 allows us to distinguish between empty and full queue in an
        array based queue implementation.
    */
    q->arr_len = qlen + 1;

    q->queue_arr = calloc(q->arr_len, sizeof(int));

    if (q->queue_arr == NULL) {
        assert(0);
        return 2;
    }

    q->num_entries = 0;

    return 0;
}

/*!

    @function free_queue

    @discussion The complement of alloc_queue().

    @param q    Pointer to a queue_t that was previously initialized with
                alloc_queue(). Call this function when you are done using the
                queue.

    @result 0 on success. Other values indicate error.
*/
int free_queue(queue_t *q) {

    if (q == NULL || q->queue_arr == NULL) {
        assert(0);
        return 1;
    }

    free(q->queue_arr);

    return 0;
}

/*!
    @function queue_is_full

    @discussion Tests if the queue is full.

    @param q    Pointer to a queue_t that was previously initialized with
                alloc_queue().

    @result Otherwise Returns 1 if the queue is full, returns 0 if it is not
            full. Other values indicate error.

*/
int queue_is_full(queue_t *q) {

    if (q == NULL || q->taili < 0 || q->headi < 0 || q->arr_len < 0) {
        assert(0);
        return -1;
    }

    return (q->taili + 1) % q->arr_len == q->headi;
}

/*!
    @function enq

    @discussion Enqueues the given entry to the tail queue.

    @param q    Pointer to a queue_t that was previously initialized with
                alloc_queue().
    @param e    The entry being added.

    @result 0 on success. Other values indicate an error.
*/
int enq(queue_t *q, int e) {
    int r;

    if (q == NULL || q->queue_arr == NULL) {
        assert(0);
        return 1;
    }

    r = queue_is_full(q);

    if (r != 0 && r != 1) {
        // queue_is_full() error.
        assert(0);
        return 2;
    }

    if (r == 1) {
        // queue is full.
        return 3;
    }

    // queue is not full, add the entry.
    q->queue_arr[q->taili] = e;

    q->taili = (q->taili + 1) % q->arr_len;

    if (q->num_entries >= q->arr_len - 1) {
        // Invalid/unexpected number of entries
        assert(0);
        return 4;
    }

    q->num_entries++;

    return 0;
}

/*!
    @function queue_is_empty

    @discussion Tests if the queue is empty.

    @param q    Pointer to a queue_t that was previously initialized with
                alloc_queue().

    @result Returns 1 if the queue is empty, returns 0 if it is not empty. Other
            values indicate error.

*/
int queue_is_empty(queue_t *q) {

    if (q == NULL || q->taili < 0 || q->headi < 0 || q->arr_len < 0) {
        assert(0);
        return -1;
    }

    return q->headi == q->taili;
}

/*!
    @function deq

    @discussion Dequeues an entry from the head of the queue.

    @param q    Pointer to a queue_t that was previously initialized with
                alloc_queue().
    @param e    Pointer in which to return the dequeued entry if the queue is
                not empty. The pointer is not touched if the queue is empty or
                if an error occurs.

    @result 0 on success. Other values indicate an error.
*/
int deq(queue_t *q, int *e) {
    int r;

    if (q == NULL || q->queue_arr == NULL || e == NULL) {
        assert(0);
        return 1;
    }

    r = queue_is_empty(q);

    if (r != 0 && r != 1) {
        // queue_is_empty() error
        assert(0);
        return 2;
    }

    if (r == 1) {
        // queue is empty, nothing to return
        return 3;
    }

    if (q->num_entries <= 0) {
        // Invalid/unexpected number of entries
        assert(0);
        return 4;
    }

    *e = q->queue_arr[q->headi];

    q->queue_arr[q->headi] = -1; // Mark: helpful for debugging

    q->headi = (q->headi + 1) % q->arr_len;

    q->num_entries--;

    return 0;
}

/*!
    @function queue_contains

    @discussion Tests if the queue contains the given entry.

    @param q    Pointer to a queue_t that was previously initialized with
                alloc_queue().
    @param e    The entry to search for.

    @result Returns 1 if the entry is in the queue, returns 0 of the entry is
    not in the queue or if the queue is empty. All other values indicate an
    error.

*/
int queue_contains(queue_t *q, int e) {
    int i = 0;
    int t, r;

    if (q == NULL || q->queue_arr == NULL) {
        return -1;
    }

    r = queue_is_empty(q);

    if(r != 0 && r != 1) {
        // queue_is_empty() error
        assert(0);
        return -2;
    }

    if(r == 1) {
        // Queue is empty
        return 0;
    }

    for(i = 0; i < q->num_entries; i++) {
        t = (q->headi + i) % q->arr_len;
        if(q->queue_arr[t] == e)
            return 1;
    }

    return 0;
}

void test_queue(void) {
    queue_t q;
    int e = -1;
    int r;

    alloc_queue(&q, 3);

    enq(&q, 2);
    enq(&q, 6);
    enq(&q, 0);

    r = queue_contains(&q, 6);

    if(r == 1)
        printf("queue contains 6\n");

    for (int i = 0; i < 3; i++) {
        if (deq(&q, &e) == 0)
            printf("%d\n", e);
    }

    free_queue(&q);
}