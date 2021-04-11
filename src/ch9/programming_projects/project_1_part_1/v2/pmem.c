/*!
    @header Physical memory related definitions.
*/

#include <stdio.h>
#include <assert.h>
#include "vm.h"

/*! @discussion Represents physical memory. */
static unsigned char p_mem[P_MEM_SIZE];

/*!
    @function p_mem_read_byte

    @discussion Returns the value of a byte in physical memory at the given
    physical address.

    @param paddr The physical address.

    @param v If successful, the value of the byte at the address.

    @result 0 if successful.
*/
int p_mem_read_byte(addr_t paddr, signed char *v) {
    if (paddr >= P_MEM_SIZE) {
        assert(0);
        return 1;
    }

    if (v == NULL) {
        assert(0);
        return 2;
    }

    *v = p_mem[paddr];
    return 0;
}

/*!
    @function p_mem_addr

    @result Pointer to physical memory (start address).
*/
unsigned char * const p_mem_addr(void) {
    return p_mem;
}