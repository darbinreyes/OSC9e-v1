/*!
    @header Definitions for reading virtual addresses from the addresses.txt
    file.
*/
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "vm.h"
#include "get_vaddrs.h"

/* Array of virtual addresses read from addresses.txt. It is given that each
   address fits in a 16-bit integer and that there are 1000 of them. */
addr_t vaddrs[MAX_NUM_V_ADDRS];
/* Number of addresses read from addresses.txt */
size_t vaddrs_len;

/*!
    @defined V_ADDR_STR_SIZE
    @discussion Size of temporary string buffer for use with fgets() when
    reading addresses.txt line by line.

    More details:
    Although the initial implementation of this program uses 16-bit virtual
    addresses the buffer size used here assumes 64-bits so that this value need
    not be updated if the number of bits in a virtual address is increased. The
    buffer size calculation is:
    2^64 - 1
    = (decimal, 20 digits) 18446744073709551615
    = (octal, 22 digits) 1777777777777777777777
    = (decimal, 2+20 digits) 0x18446744073709551615 // Allowing for leading 0x
    = (octal, 1+22 digits) 01777777777777777777777  // Allowing for leading 0
    = (decimal, 1+2+20 digits) -0x18446744073709551615
    // Allowing for leading 0x and sign
    = (octal, 1+1+22 digits) -01777777777777777777777
    // Allowing for leading 0 and sign
    Thus we conclude 1+1+22 = 24 characters are required to read a virtual
    address value as a string. Adding +1 for a null terminator and +1 for a
    potential new line character yields a final total of 26 characters.
    Finally,the value is arbitrarily increased to 32, the next nearest power of
    2.
*/
#define V_ADDR_STR_SIZE (32U)

/*!
    @function init_vaddrs

    @discussion Initializes the vaddrs array according to the contents of the
    given filename.

    @param vaddrs_fname The filename of the file from which to read the virtual
    addresses e.g. addresses.txt.

    @result 0 if successful. Otherwise error, the program should terminate.
*/
int init_vaddrs(const char *vaddrs_fname) {
    FILE *vaddrs_fp = NULL;
    size_t i = 0, ln = 1;
    unsigned long t_vaddr = 0;
    char *endptr = NULL;
    char t_addr_str[V_ADDR_STR_SIZE];

    if (vaddrs_fname == NULL) {
        assert(0);
        return 1;
    }

    errno = 0; // Per man page.
    vaddrs_fp = fopen (vaddrs_fname, "r");

    if (vaddrs_fp == NULL) {
        fprintf(stderr, "fopen(\"r\") returned NULL! filename = %s. "\
                "error = %s.\n", vaddrs_fname, strerror(errno));
        return 2;
    }

    errno = 0;
    while (!feof(vaddrs_fp)) {

        if(!fgets(t_addr_str, sizeof(t_addr_str), vaddrs_fp)) {
            // EOF or error occurred.

            /* FYI: If the file ends with a empty line then fgets() must be
               called a number of times equal to the number of virtual addresses
               in the file plus one in order to reach EOF. Hence this break.*/
            printf("FYI: fgets() returned NULL.\n");
            break;
        }

        printf("line # %lu.\n", ln++);

        if (!strcmp(t_addr_str, "\n")) // Skip blank lines.
            continue;

        if (*t_addr_str == '\0') {
            fprintf(stderr, "Unexpected empty string!\n");
            return 8;
        }

        if (t_addr_str[strlen(t_addr_str) - 1] == '\n')
            printf("line is %s", t_addr_str);
        else
            printf("line is %s\n", t_addr_str);

        // FYI: strtol() with base = 0 provides handling for hex and octal.
        errno = 0;
        t_vaddr = strtoul(t_addr_str, &endptr, 0);

        if (t_addr_str == endptr) {
            fprintf(stderr, "strtoul() found no digits at all! error = %s.\n", \
                   strerror(errno));
            return 3;
        }

        if (*endptr != '\n' && *endptr != '\0') {
            fprintf(stderr, "strtoul() returned unexpected endptr!\n");
            return 4;
        }

        if (errno != 0 && t_vaddr == ULONG_MAX) {
            fprintf(stderr, "strtoul() overflowed! " \
                    "error = %s.\n", strerror(errno));
            return 5;
        }

        if (t_vaddr > MAX_V_ADDR) {
            fprintf(stderr, "Virtual address out of range (> %lu)! %lu "\
                    "= 0x%lX\n", MAX_V_ADDR, t_vaddr, t_vaddr);
            return 6;
        }

        if (i >= MAX_NUM_V_ADDRS) {
            fprintf(stderr, "Number of virtual addresses exceeds max allowed "\
                             "(%d).\n", MAX_NUM_V_ADDRS);
            return 7;
        }

        vaddrs[i] = (addr_t) t_vaddr;
        printf("Virtual address # %lu. %lu = 0x%lX\n", i, vaddrs[i], vaddrs[i]);
        i++;

    }

    vaddrs_len = i;

    if (feof(vaddrs_fp) && ferror(vaddrs_fp)) {
        /* fgets() function does not distinguish between end-of-file and error,
           and callers must use feof(3) and ferror(3) to determine which
           occurred. */
        fprintf(stderr, "fgets() error! filename = %s. error = %s.\n", \
                vaddrs_fname, strerror(errno));
        return 9; // Bail. FYI: The OS will close the file.
    }

    errno = 0;
    if (fclose(vaddrs_fp)) {
        fprintf(stderr, "fclose() returned error! filename = %s. "\
                "error = %s.\n", vaddrs_fname, strerror(errno));
        return 9;
    }

    return 0;
}