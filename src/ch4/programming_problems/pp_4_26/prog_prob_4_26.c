#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define FIB_ARRAY_SIZE (2<<20) // 1,048,576

static long double fib_a[FIB_ARRAY_SIZE]; // Use long double because the fib. sequence overflows smaller arithmetic type very quickly e.g. int already overflows at 50 numbers.

void *fib(void *param) {
    int n;

    n = *((int *)param);

    fib_a[0] = 0; /* The first two numbers are given constants. */
    fib_a[1] = 1;

    for (int i = 2; i < n; i++) {
        fib_a[i] = fib_a[i - 1] + fib_a[i - 2]; // f_n+1 = f_n + f_n-1.
    }

    return NULL;
}

int main(int argc, char **argv) {
    int n;
    pthread_t tid;
    pthread_attr_t attr;

#define ARG_ERROR_MSG "USAGE: Enter a single positive integer less than 1,048,576."\
                      " Example: `./a.out 7`.\n"

    if (argc != 2) {
        printf(ARG_ERROR_MSG);
        return 1;
    }

    n = atoi(argv[1]);

    if (n <= 0) {
        printf(ARG_ERROR_MSG);
        return 2;
    }

    if (n > FIB_ARRAY_SIZE) {
        printf(ARG_ERROR_MSG);
        return 3;
    }

    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, fib, &n);
    pthread_join(tid, NULL);

    for(int i = 0; i < n; i++) {
        if (i == n - 1)
            printf("%.1Lf.\n", fib_a[i]);
        else
            printf("%.1Lf, \n", fib_a[i]);
    }

    return 0;
}