/*!
    @header Page replacement algorithm simulator.
    Programming Problem 9.39 from Operating System Concepts, Galvin.

*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include "page_repl.h"
#include "args.h"
#include <errno.h>

extern const char * usage_str;

/*!
    @function rand_ref_str

    @discussion Fills reference string ref_str with len random integers
    in the range 0-9.

    @param len Length of ref_str array.

    @param ref_str Array of integers to be filled with random numbers in range
                   0-9.

    @result 0 if successful. Otherwise an error occurred.
*/
int rand_ref_str(int len, int *ref_str) {

    if (len <= 0 || ref_str == NULL) {
        return 1;
    }

    sranddev();

    for(int i = 0; i < len; i++) {
        ref_str[i] = rand()/(RAND_MAX/10 + 1);
    }

    return 0;
}

/*!
    @function get_ref_str_len

    @discussion Returns the length of a list of integers given as a string e.g.
    if ref_str = "2 3 5" then *len == 3.

    @param ref_str Reference string encoded as a character string e.g.
    "2 3 5". Must contain only non-negative integers. Must contain only digits
    and spaces.

    @param len If successful, upon return, the length of the reference
    string. e.g. if ref_str = "2 3 5" then *len == 3. Not touched if an error
    occurs.

    @result 0 if successful. Otherwise an error occurred, in which case the
    program should bail.
*/
int get_ref_str_len(const char *ref_str, int * const len) {
    char *endptr = NULL;
    const char *s;
    int l = 0;
    long t;

    if (ref_str == NULL || len == NULL) {
        return 1;
    }

    if (*ref_str == '\0') // Reject empty string.
        return 2;

    s = ref_str; // keep pointer to first char.

    // Make sure we only have digits and spaces.
    while (*ref_str != '\0') {
        if(!isdigit(*ref_str) && !isspace(*ref_str))
            return 3;

        ref_str++;
    }

    ref_str = s; // Reset

    while (*ref_str != '\0') {
        t = strtol(ref_str, &endptr, 10);

        if (t < 0)
            return 4; // Shouldn't happen, we rejected '-' chars above.

        if (t == 0 && errno == EINVAL)
            return 5; // Conversion failed.

        if (errno == ERANGE)
            return 6; // Over/under-flow.

        if (endptr == NULL) // Sanity check, should not occur.
            return 7;

        l++;

        ref_str = endptr; // endptr should point to a space char or '\0'
    }

    *len = l;

    return 0;
}

/*!
    @function ref_str_to_int_arr

    @discussion Converts a list of integers given in string format to an array
    of integers. This function assumes that get_ref_str_len() has been called to
    calculate the len argument, that function does **essential** validation on
    ref_str.

    @param ref_str Reference string encoded as a character string e.g.
    "2 3 5". Must contain only non-negative integers. Must contain only digits
    and spaces.

    @param len Length of ref_str as obtained from get_ref_str_len() e.g.
     if ref_str = "2 3 5" then len must be == 3.

    @param int_ref_str An integer array with length equal to len.

    @result 0 if successful. Otherwise an error occurred.
*/
int ref_str_to_int_arr(const char *ref_str, int len, int *int_ref_str) {
    char *endptr = NULL;
    const char *s;
    int l = 0;
    long t;
    int i = 0;

    if (ref_str == NULL || len <= 0 || int_ref_str == NULL) {
        return 1;
    }

    while (*ref_str != '\0') {

        if (len-- == 0)
            return 2; // Converted more integers than expected.

        t = strtol(ref_str, &endptr, 10);

        *int_ref_str = (int) t;
        int_ref_str++;

        if (endptr == NULL) // Sanity check, should not occur.
            return 3;

        ref_str = endptr; // endptr should point to a space char or '\0'
    }

    return 0;
}

/* Reference string from exercise 9.8 - the page fault counts match the
   solution. */
static char *fixed_ref_str_arg = "1 2 3 4 2 1 5 6 2 1 2 3 7 6 3 2 1 2 3 6";

/*!
    @function print_ref_str

    @discussion Prints the reference string and its length. Here length means
    the number of integers in the list of integers in ref_str e.g. if
    ref_str = "2 3 5" then its length is 3.

    @param ref_str The reference string.

    @param len  The length of the reference string. Here length means the number
    of integers in the list of integers in ref_str e.g. if ref_str = "2 3 5"
    then its length is 3.

    @result 0 if successful. Otherwise an error occurred.

*/
int print_ref_str(int *ref_str, int len) {

    if(ref_str == NULL || len <= 0) {
        assert(0);
        return 1;
    }

    printf("reference string length = %d.\n", len);
    printf("reference string =\n");

    for(int i = 0; i < len; i++) {
        printf("%d ", ref_str[i]);
    }

    printf("\n");

    return 0;
}

/*!
    @function run_cmd

    @discussion Runs as specified by the user with command line arguments.

    @param ref_str The reference string as an integer array e.g. {2, 3, 5, 7}.
    Must not be empty, must all be non-negative.

    @param len The number of integers in ref_str. Must be non-negative.

    @param npf The number of page frames to use.


*/
int run_cmd(int *ref_str, int len, int npf) {
    int p0, p1, p2;
    int npf_l = 1, npf_h = 8;

    if(ref_str == NULL || len <= 0) {
        assert(0);
        return 1;
    }

    if (npf > 0) {
        npf_l = npf;
        npf_h = npf + 1;
    }

    //printf("Running in mode 0.\n");

    print_ref_str(ref_str, len);

    printf("# page frames | FIFO | LRU | OPT\n");

    for(int i = npf_l; i < npf_h; i++) {
        p0 = do_fifo(len, ref_str, i);
        p1 = do_lru(len, ref_str, i);
        p2 = do_opt(len, ref_str, i);
        printf("%13d | %4d | %3d | %3d\n", i, p0, p1, p2);
    }

    return 0;
}

int main(int argc, char **argv) {
    int i;
    int len = 25;
    int num_page_frames = -1;
    int *ref_str = NULL;
    char *arg_name = NULL;
    int use_rand = 0;
    int use_rs = 0;

    if (argc == 1) { // Usage: a.out
        if(get_ref_str_len(fixed_ref_str_arg, &len))
            return 2;

        ref_str = calloc(len, sizeof(int));

        if(ref_str == NULL) {
            assert(0);
            return 1;
        }

        if(ref_str_to_int_arr(fixed_ref_str_arg, len, ref_str)) {
            free(ref_str);
            return 3;
        }

        run_cmd(ref_str, len, -1);
        free(ref_str);
        return 0;
    }

    arg_name = "-npf"; // Number of page frames

    if(get_arg_pi(argc, argv, &num_page_frames, arg_name) < 0) {
        printf("Error getting %s arg.\n", arg_name);
        return 4;
    }

    if (argc == 3 && num_page_frames > 0) { // Usage: a.out -npf 7
        if(get_ref_str_len(fixed_ref_str_arg, &len))
            return 2;

        ref_str = calloc(len, sizeof(int));

        if(ref_str == NULL) {
            assert(0);
            return 1;
        }

        if(ref_str_to_int_arr(fixed_ref_str_arg, len, ref_str)) {
            free(ref_str);
            return 3;
        }

        run_cmd(ref_str, len, num_page_frames);
        free(ref_str);
        return 0;
    }

    arg_name = "-rand";

    if (get_arg_bool(argc, argv, &use_rand, arg_name) < 0) {
        printf("Error getting %s arg.\n", arg_name);
        return 6;
    }

    if(argc == 4 && num_page_frames > 0 && use_rand) { // Usage: a.out -npf 9 -rand
        len = 20; // Default length of random reference string.
        ref_str = calloc(len, sizeof(int));

        if(ref_str == NULL) {
            return 1;
        }

        if(rand_ref_str(len, ref_str)) {
            free(ref_str);
            return 2;
        }

        run_cmd(ref_str, len, num_page_frames);
        free(ref_str);
        return 0;
    }

    arg_name = "-rsl"; // Reference string length

    len = -1;

    if(get_arg_pi(argc, argv, &len, arg_name) < 0) {
        printf("Error getting %s arg.\n", arg_name);
        return 7;
    }

    if(argc == 6 && num_page_frames > 0 && use_rand && len > 0) {  // Usage: a.out -npf 10 -rand -rsl 31
        ref_str = calloc(len, sizeof(int));

        if(ref_str == NULL) {
            printf("calloc() returned NULL! Bye!\n");
            return 1;
        }

        if(rand_ref_str(len, ref_str)) {
            free(ref_str);
            return 2;
        }

        run_cmd(ref_str, len, num_page_frames);
        free(ref_str);
        return 0;
    }

    arg_name = "-rs"; // Given reference string

    if (get_arg_bool(argc, argv, &use_rs, arg_name) < 0) {
        printf("Error getting %s arg.\n", arg_name);
        return 8;
    }

    // @TODO Usage: a.out -npf 11 -rs '1 2 3 4 5 1 2 3 6'

    if (argc == 5 && num_page_frames > 0 && use_rs) {
        if(get_ref_str_len(argv[use_rs + 1], &len)) // Assumes the reference string is provided after the -rs argument.
            return 2;

        ref_str = calloc(len, sizeof(int));

        if(ref_str == NULL) {
            assert(0);
            return 1;
        }

        if(ref_str_to_int_arr(argv[use_rs + 1], len, ref_str)) {
            free(ref_str);
            return 3;
        }

        run_cmd(ref_str, len, num_page_frames);
        free(ref_str);
        return 0;
    }

    printf("%s", usage_str);
    printf("Unsupported command line arg. combination. Bye!\n");

    return 9;
}