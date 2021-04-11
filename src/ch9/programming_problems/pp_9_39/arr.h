#ifndef __ARR_H__
#define __ARR_H__

typedef struct _arr_t {
    int *arr;
    int arr_len;
    int num_entries;
} arr_t;

int alloc_arr(arr_t *a, int arr_len);

int free_arr(arr_t *a);

int arr_contains(arr_t *a, int e);

int arr_add(arr_t *a, int e);

#endif