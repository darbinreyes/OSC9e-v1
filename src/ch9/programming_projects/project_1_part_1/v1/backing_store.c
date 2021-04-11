/*!
    @header Definitions related to the backing store. Mainly reading pages from
    BACKING_STORE.bin.
*/

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include "vm.h"

/*!
    @defined BACKING_STORE_FNAME
    @discussion Default filename representing the backing store.
*/
#define BACKING_STORE_FNAME "BACKING_STORE.bin"

/*!
    @function backing_store_read

    @discussion Reads a page (page_num) from the backing store into the
    specified address in physical memory (dst).

    @param page_num Identifies the location of a page on the backing to read
    from.
    @param dst The destination address in physical memory for the read.

    @result 0 if successful.
*/
int backing_store_read(addr_t page_num, unsigned char *dst) {
    /*! @discussion Temporary buffer used for destination of a read operation on
        the backing store. */
    static unsigned char page_io_buffer[PAGE_SIZE];
    const char *bs_fname = BACKING_STORE_FNAME;
    static FILE *bs_fp;
    size_t nb;

    if (dst == NULL) {
        assert(0);
        return 1;
    }

    if (page_num >= NUM_BS_PAGES) {
        fprintf(stderr, "page_num is out of bounds for backing store. " \
                "%lu >= %lu.\n", page_num, NUM_BS_PAGES);
        assert(0);
        return 1;
    }

    if (!bs_fp) { // Backing store file not open yet.
        errno = 0;
        bs_fp = fopen(bs_fname, "r");
        if (!bs_fp) {
            fprintf(stderr, "fopen(\"r\") returned NULL! filename = %s. "\
                    "error = %s.\n", bs_fname, strerror(errno));
            assert(0);
            return 2;
        }
    }

    errno = 0;
    if(fseek(bs_fp, page_num * PAGE_SIZE, SEEK_SET)) {
        fprintf(stderr, "fseek() error!  filename = %s. error = %s.\n", \
                bs_fname, strerror(errno));
        assert(0);
        return 3;
    }

    errno = 0;
    nb = fread(page_io_buffer, 1, sizeof(page_io_buffer), bs_fp);
    if (nb < sizeof(page_io_buffer) && ferror(bs_fp)) {
        fprintf(stderr, "fread() short byte count + error. filename = %s. "
                "error = %s.\n", bs_fname, strerror(errno));
        assert(0);
        return 4;
    }

    if (feof(bs_fp) && ferror(bs_fp)) {
        fprintf(stderr, "fread() error! filename = %s. error = %s.\n", \
                bs_fname, strerror(errno));
        assert(0);
        return 5;
    }

    memcpy(dst, page_io_buffer, sizeof(page_io_buffer));

    return 0;
}