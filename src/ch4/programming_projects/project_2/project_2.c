#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "sort.h"
#include <pthread.h>



static int *in_a = NULL; // Array to be sorted.
static int in_len = 0;   // Length of `in_a`.

static int *out_a = NULL; // Sorted array.

typedef
struct _sort_param_t {
    int *a;
    int l;
} sort_param_t;

typedef
struct _merge_param_t {
    int *am;
    int aml;

    int *a0;
    int a0l;

    int *a1;
    int a1l;
} merge_param_t;

void *sort_runner(void *param) {
    sort_param_t *p;

    assert(param != NULL);

    p = (sort_param_t *)param;
    sort(p->a, p->l);

    return NULL;
}

void *merge_runner(void *param) {
    merge_param_t *p;

    assert(param != NULL);

    p = (merge_param_t *) param;

    merge(p->am, p->aml, \
           p->a0, p->a0l, \
           p->a1, p->a1l);
    return NULL;
}

/* Default array to be sorted if none is given on the command line. */
static int in_a_default[] = {7, 12, 19, 3, 18, 4, 2, 6, 15, 8};
#define IN_ARRAY_DEFAULT_LEN (sizeof(in_a_default)/sizeof(in_a_default[0]))


#define IN_ARRAY_LEN_2 1024 // Max length of array given on command line.


static int in_a2[IN_ARRAY_LEN_2];

static int out_a2[IN_ARRAY_LEN_2];

void get_input(int argc, char **argv) {
    /* This function does not handle invalid input from the command line.
    The input should be a list of space separated integers. */
    assert(argc >= 0 && argv != NULL);

    out_a = out_a2;

    if (argc <= 1) {
        in_a = in_a_default;
        in_len = IN_ARRAY_DEFAULT_LEN;
        return;
    }

    in_a = in_a2;
    in_len = argc - 1;
    argv++;
    for (int i = 0; i < in_len; i++) {
        in_a2[i] = atoi(argv[i]);
    }
}

#define NUM_THREADS 3

int main(int argc, char **argv) {
    pthread_t tid[NUM_THREADS];
    pthread_attr_t attr[NUM_THREADS];
    int *am, *a0, *a1;
    int am_len, a0_len, a1_len;
    int i;
    sort_param_t sp0, sp1;
    merge_param_t mp;

    get_input(argc, argv);

    // TODO: handle len == 1
    assert(in_len > 1);

    printf("Input array: ");
    print_a(in_a, in_len);

    for (i = 0; i < NUM_THREADS; i++)
        pthread_attr_init(&attr[i]);

    sp0.a = in_a;
    sp0.l = in_len/2;
    pthread_create(&tid[0], &attr[0], sort_runner, &sp0);

    sp1.a = in_a + in_len/2;
    sp1.l = in_len - in_len/2;
    pthread_create(&tid[1], &attr[1], sort_runner, &sp1);

    for (i = 0; i < 2; i++)
        pthread_join(tid[i], NULL);

    mp.am = out_a;
    mp.aml = in_len;
    mp.a0 = in_a;
    mp.a0l = in_len/2;
    mp.a1 = in_a + in_len/2;
    mp.a1l = in_len - in_len/2;
    pthread_create(&tid[2], &attr[2], merge_runner, &mp);
    pthread_join(tid[2], NULL);

    print_a(out_a, in_len);

    return 0;
}

