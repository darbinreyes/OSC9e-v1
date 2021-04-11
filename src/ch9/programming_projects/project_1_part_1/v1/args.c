/*!
    @header Command-line argument related definitions.
*/
#include <stdio.h>
#include <assert.h>
#include "vm.h"

/*!
    @defined DEFAULT_ADDRS_FNAME
    @discussion Default filename containing the virtual addresses to be
    translated.
*/
#define DEFAULT_ADDRS_FNAME "addresses.txt"

/*!
    @function valid_int_sizes
    @discussion Verifies that the size of integers and virtual address
    definitions are valid/as expected.
    @result 0 if valid. Otherwise not valid.
*/
int valid_int_sizes(void) {
    // Verify the size of integers.
    if (sizeof(short) != 2 || sizeof(int) != 4 || sizeof(long) != 8) {
        fprintf(stderr, "Unexpected int size.\n");
        assert(0);
        return 1;
    }
    /* This program only deals with virtual addresses less than 64-bits wide. */
    if (!(ADDR_NBITS < sizeof(addr_t) * 8) || !(PAGE_OFFSET_NBITS < ADDR_NBITS)) {
        fprintf(stderr, "Unsupported virtual address size/def.\n");
        assert(0);
        return 1;
    }

    return 0;
}

/*!
    @function valid_args
    @discussion General checks that the args passed to main() are valid.
    @param argc The usual main() arg.
    @param argv The usual main() arg.
    @result 0 if valid. Otherwise not valid.
*/
int valid_args (int argc, const char * const * const argv) {
    if (argc < 1 || argv == NULL) {
        assert(0);
        return 1;
    }

    if (argc > 2) {
        fprintf(stderr, \
                "Usage 1: ./a.out. The default input filename is "\
               "addresses.txt.\n"\
                "Usage 2: ./a.out addresses.txt. addresses.txt can be any "\
                "filename.\n");
        return 1;
    }

    return 0;
}

/*!
    @function get_arg_vaddrs_filename

    @discussion Gets the addresses.txt filename argument.

    @param argc The usual main() arg.
    @param argv The usual main() arg.
    @param fname The filename if successful.

    @result 0 if successful. Error otherwise.
*/
int get_arg_vaddrs_filename (int argc, const char * const * const argv, \
                             const char ** const fname) {
    const char *vaddrs_fname = DEFAULT_ADDRS_FNAME;

    if (argc < 1 || argc > 2 || argv == NULL || fname == NULL) {
        assert(0);
        return 1;
    }

    if (argc == 2) { // Use given filename instead of default.
        vaddrs_fname = argv[1];
    }

    *fname = vaddrs_fname;

    return 0;
}