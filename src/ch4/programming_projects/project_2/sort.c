#include <stdio.h>
#include <assert.h>

// Returns the index of the minimum element in a[]. Returns -1 on error.
int get_min_index(int a[], unsigned int len) {
    unsigned int min, min_index, i;

    if (a == NULL || len == 0) {
        assert(0);
        return -1;
    }

    min_index = 0;
    min = a[min_index];

    for (i = 1; i < len; i++) {
        if (a[i] < min) {
            min_index = i;
            min = a[i];
        }
    }

    return min_index;
}

// Returns 0 if successful. Returns 1 on error.
int swap(int a[], unsigned int i, unsigned int j, unsigned int len) {
    int t;

    if (a == NULL || i >= len || j >= len) {
        assert(0);
        return 1;
    }

    t = a[i];
    a[i] = a[j];
    a[j] = t;

    return 0;
}

void sort(int a[], unsigned int len) {
    int m, r, i;

    for (i = 0; i < len - 1; i++) {
        m = get_min_index(&a[i + 1], len - i - 1); // Find the index of the min. element in the unsorted subset of the array.
        assert(m != -1);
        m += i + 1;
        if (a[i] > a[m]) {
            r = swap (a, i, m, len);
            assert(r == 0);
        }
    }
}

void merge(int am[], const unsigned int am_len, \
           int a0[], const unsigned int a0_len, \
           int a1[], const unsigned int a1_len) {
    assert(am != NULL & a0 != NULL & a1 != NULL);
    assert(am_len >= a0_len + a1_len && a0_len > 0 && a1_len > 0);
    unsigned int i = 0, j = 0, k = 0;

    /* Merge sorted arrays a0 and a1 into a single array am. */
    while (i < a0_len && j < a1_len) {
        /* Inspect a0 and a1 for the minimum element. */
        if (a0[i] < a1[j]) {
            am[k] = a0[i];
            k++;
            i++;
        } else if (a0[i] == a1[j]) {
            am[k] = a0[i];
            k++;
            i++;
            am[k] = a1[j];
            k++;
            j++;
        } else { // a0[i] > a1[j]
            am[k] = a1[j];
            k++;
            j++;
        }
    }

    /* One of the arrays may contain elements which were not placed in am, in
    this case all we need to do this copy over the remaining elements into am. */
    while (i < a0_len) {
        am[k] = a0[i];
        k++;
        i++;
    }

    while (j < a1_len) {
        am[k] = a1[j];
        k++;
        j++;
    }
}

void print_a(int a[], unsigned int len) {

    assert(a != NULL && len > 0);

    for (int i = 0; i < len; i++) {
        printf("%d ", a[i]);
    }

    printf("\n");
}