#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

// Returns 1 if p is a prime number. 0 otherwise.
int is_prime(long p) {
    long sqr;

    if (p < 2)
        return 0;
    if (p < 4)
        return 1;

    // assert p >= 4

    sqr = (long) sqrt((double) p);

    for (long i = 2; i <= sqr; i++) {
        if (p % i == 0)
            return 0;
    }

    return 1;
}

void *runner(void *param) {
    long upper_bound;
    upper_bound = *((long *) param);

    /* Print all primes <= upper_bound*/

    for (long i = 2; i <= upper_bound; i++) {
        if (is_prime(i))
            printf("%ld ", i);
    }

    printf("\nDone.\n");

    return NULL;
}

int main(int argc, char **argv) {
    long upper_bound;
    char *endp;
    pthread_attr_t attr;
    pthread_t tid;

#define USAGE_STR "Usage: Provide a single integer > 0. The integer must fit in a long type. Example: ./a.out 78.\n"

    if (argc < 2) {
        printf(USAGE_STR);
        return 1;
    }

    if (argc > 2) {
        printf(USAGE_STR);
        return 1;
    }

    // assert argc == 2.

    upper_bound = strtol(argv[1], &endp, 10);

    /*  Enforce that the last character of the integer command line argument is
    also the last character of the entire command. Also, enforce that the integer
    is > 0. */
    if (*endp != '\0' || upper_bound <= 0) {
        printf(USAGE_STR);
        return 1;
    }

    // Input validated. Compute.

    // Create thread, join thread.

    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, runner, &upper_bound);
    pthread_join(tid, NULL);


    return 0;
}


