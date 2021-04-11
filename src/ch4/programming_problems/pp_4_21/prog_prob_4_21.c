#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ARRAY_SIZE 10

long a[ARRAY_SIZE];
int as;

long avg, min, max;

void *avg_thread(void *param) {

    avg = 0;

    for (int i = 0; i < as; i++) {
        avg += a[i]; // FYI: Not checking for overflow.
    }

    avg /= as;

    return NULL;
}

void *min_thread(void *param) {
    long t;

    t = a[0];
    for (int i = 1; i < as; i++) {
        if (a[i] < t)
            t = a[i];
    }

    min = t;

    return NULL;
}

void *max_thread(void *param) {

    long t;

    t = a[0];
    for (int i = 1; i < as; i++) {
        if (a[i] > t)
            t = a[i];
    }

    max = t;

    return NULL;
}

#define NUM_THREADS 3

typedef
void *(*stat_thread_func)(void *param);

int main(int argc, char **argv) {
    pthread_t tid[NUM_THREADS];
    pthread_attr_t attr[NUM_THREADS];
    stat_thread_func thread_funcs[NUM_THREADS] = {avg_thread, min_thread, max_thread};
    int i;

    if (argc <= 1) {
        printf("Usage: Provide up to %d positive integers. "
               "More than that will be ignored. Example ./a.out 2 3 5 7.\n", ARRAY_SIZE);
        return 1;
    }

    for (i = 0; i < ARRAY_SIZE && i < argc - 1; i++) {
        a[i] = strtol(argv[i+1], NULL, 10);
        if (a[i] == 0) {
            printf("Failed to convert \"%s\" to an integer. Aborted.\n", argv[i+1]);
            return -1;
        }
    }

    as = i;

    printf("%d inputs: ", as);

    for (i = 0; i < as; i++) {
        printf("%ld ", a[i]);
    }

    printf("\n");

    for(i = 0; i < NUM_THREADS; i++) {
        pthread_attr_init(&attr[i]);
        pthread_create(&tid[i], &attr[i], thread_funcs[i], NULL);
    }

    for(i = 0; i < NUM_THREADS; i++) {
        pthread_join(tid[i], NULL);
    }

    printf("avg. %ld\nmin. %ld\nmax. %ld\n", avg, min, max);

    return 0;
}