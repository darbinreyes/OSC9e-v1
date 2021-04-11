/*!
    @header Array Interface.
    A simple interface to an array ADT to facilitate the implementation of the
    OPT page replacement algorithms.
*/

#include <stdlib.h>
#include <assert.h>
#include "arr.h"

/*!
    @function alloc_arr

    @discussion Initializes
    a->arr = calloc(arr_len, sizeof(int));
    a->arr_len = arr_len;
    a->num_entries = 0;

    @param a    Pointer to an arr_t.

    @param arr_len    Length of the a->arr array to allocated.

    @result 0 on success. Otherwise an error occurred.
*/
int alloc_arr(arr_t *a, int arr_len) {

    if (a == NULL || arr_len <= 0) {
        assert(0);
        return 1;
    }

    a->arr = calloc(arr_len, sizeof(int));

    if(a->arr == NULL) {
        assert(0);
        return 2;
    }

    a->arr_len = arr_len;
    a->num_entries = 0;

    return 0;
}

/*!
    @function free_arr

    @discussion Frees memory allocated by a successful call to alloc_arr().

    @param a    Pointer to an arr_t.

    @result 0 on success. Otherwise an error occurred.
*/
int free_arr(arr_t *a) {

    if(a == NULL || a->arr == NULL) {
        assert(0);
        return 1;
    }

    free(a->arr);

    return 0;
}

/*!
    @function arr_contains

    @discussion Tests if entry e is in the array. The search terminates as soon
    as the first instance of e is found. Only entries inside of a->arr[0] to
    a->arr[a->num_entries - 1] are searched.

    @param a    Pointer to an arr_t

    @param e    The entry to search for.

    @result 1 if e is found in the array, 0 if e is not found in the array.
    Other values indicate error.

*/
int arr_contains(arr_t *a, int e) {
    int i;

    if (a == NULL || a->arr == NULL || a->num_entries < 0) {
        assert(0);
        return -1;
    }

    for (i = 0; i < a->num_entries; i++) {
        if(a->arr[i] == e)
            return 1;
    }

    return 0;
}

/*!
    @function  arr_add

    @discussion Add an entry to the array if there is space.

    @param a    Pointer to an arr_t

    @param e    Entry to add to the array.

    @result 0 if the entry was successfully added. Other values indicate an
    error.
*/
int arr_add(arr_t *a, int e) {

    if(a == NULL || a->arr == NULL) {
        assert(0);
        return 1;
    }

    if(a->arr_len <= 0) {
        assert(0);
        return 2;
    }

    if(a->num_entries < 0) {
        assert(0);
        return 3;
    }

    if (a->num_entries >= a->arr_len) {
        return 4; // No space
    }

    a->arr[a->num_entries] = e;
    a->num_entries++;

    return 0;
}