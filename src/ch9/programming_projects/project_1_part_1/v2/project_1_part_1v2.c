/*!

    @header Chapter 9 Programming Project: Designing a Virtual Memory Manager
    @discussion
    * This implementation started with `v1/project_1_part_1v1.c`
      and was modified to fix a bug in the TLB entry replacement.
    * What follows in this block comment is mostly identical to
      what is in `v0/project_1_part_1v0.c`. The main difference is in the
      correction of typos.
    * A program that translates logical to physical addresses.
    * Virtual address space size: 2^16 = 65536.
    * Given a file containing **logical addresses**.
    * We will simulate the use of a TLB and a page table.
    * Each logical address will be translated to its physical address.

# Specifics

    * The given file containing logical addresses is a list of
      **32-bit integers**.
    * You should mask off the rightmost 16-bits of each logical address.
    * The 16-bit values are to be divided into an 8-bit page number and an 8-bit
      page offset: viz. bits[15:8] = page number bits[7:0] = offset.
    * Other specifics:
    * 2^8 entries in the page table.
      (= size_of_virtual_address_space/size_of_a_page = 2^16/2^8 = 2^8 = 256).
    * Page size of 2^8 bytes.
      (= 2^(number_of_bits_in_page_offset_field_of_logical_address)).
    * A TLB with space for 16 entries. (=2^4).
      (Key=Page Number, Value=Frame Number).
    * Frame size of 2^8 bytes. (frame size always = page size = 2^8 bytes).
    * 256 frames. (This implies that the physical address space =
      number_of_frames * frame_size = 2^8 * 2^8 = 2^16 bytes, therefore the
      virtual address space size == the physical address space size.)
    * Physical memory of 65536 bytes = 2^16 bytes. (Hence above bullet point).
    * Your program only needs to support reading physical addresses, not writing
      to physical addresses.

# Address Translation

    * Again, translation will involve both a TLB and a page table.
    * 1. Extract page number from logical address. (Read address from a line in
      file addresses.txt, mask out the low order 16 bits, take bits[15:8] as the
      page number).
      2. Consult the TLB to translate the page number to a frame number.
      If we have a TLB-hit, the TLB provides the frame number, the physical
      address = <frame_number , page offset>.
      else, we have a TLB-miss, we consult the page-table for the frame number,
      again the physical address = <frame_number , page offset>.
      If the page table does not contain a frame number, a **page fault** has
      occurred. Thus the page is not in memory, it must be read in from the
      backing store into physical memory.

# Handling Page Faults

    * The program will implement demand paging, i.e. a page is read into
      physical memory from the backing store only when a page fault occurs.
    * The backing store is represented by the given file BACKING_STORE.bin. This
      is a binary file 65536 bytes in size.
    * When a page fault occurs, a 256-byte page should be read in from the .bin
      file into physical memory.
    * Example: Page number 15 results in a page fault -> read in page number 15
      from the backing store into physical memory -> update the page table and
      TLB. All subsequent accesses to page number 15 should be resolved in the
      TLB or the page table. (After being brought into memory, page number 15
      should never result in a page fault, since this project specifies that the
      physical address space size = the virtual address space size, therefore no
      page ever gets kicked out of physical memory. In practice, the physical
      address space size is the lesser, and we must replace pages in physical
      memory according to some page replacement strategy, such as FIFO, LRU,
      MRU, LFU, MFU, or ideally "OPT". Furthermore, this project specifies a
      very simplified algorithm for locating a page on the backing store when a
      page fault occurs, viz. the size of the virtual address space = the size
      of the backing store, and the page resides on the backing store exactly
      where it resides in the virtual address space. In practice, a page fault
      causes a trap to the OS, and the OS consults some internal table
      associated with the page faulting process that indicates where on the
      backing store the page resides e.g. if the backing store is a hard disk,
      then some sort of cylinder-head-sector based address.)
    * It is given that you treat BACKING_STORE.bin as a random access file using
      the standard C lib. functions fseek(), fread(), etc. .
    * This part of the project does not require the use of a page replacement
      strategy since the virtual address space size = the physical address space
      size, and all of physical address space is available for demand paging. In
      practice, part of physical memory must be used for the e.g. OS, and MMI/O.

# Test File

    * addresses.txt contains the logical addresses to be translated, one address
      per line. The byte value read from the corresponding physical address
      should be output as a signed char value.

# How to Begin

    * Start by obtaining the page number and page offset.
    * Start without a TLB, use a page table only, add the TLB after you verify
    the program is correct using the page table only. Recall the TLB only speeds
    things up, like normal CPU caches.
    * Recall that the TLB only has 16 entries, so a replacement strategy will be
      required: use FIFO or LRU.

# How to Run Your Program

    * Example: `./a.out addresses.txt`
    * addresses.txt contains 1000 logical addresses.
    * The program should output 3 values as indicated in the file correct.txt.

# Statistics

    * The program should report:
    1. page-fault rate = percent of address references that result in a page
    fault.
    2. TLB-hit rate = percent of address references that were resolved in the
    TLB. (My remark: nice to have, implement and compare FIFO vs. LRU TLB entry
    replacement).
    * It is noted that addresses.txt was generated randomly, therefore do not
      expect a high TLB hit rate. In practice, real programs exhibit memory
      access locality.

# My Remarks

    * It is manifest that it is up to me to decide where in physical memory a
      page being read in from the backing store should be placed. The
      simplest choice seems to be the next free page of physical memory, we can
      simply fill physical memory starting from lowest page number up the
      highest page number.

# Modifications

    * "Suggested": use a physical address space that is smaller than the virtual
      address space, specifically, make the physical address space 128 page
      frames instead of 256 page frames. This will require that you keep track
      of free page frames and that you implement a page replacement strategy:
      use either FIFO or LRU page replacement.
    * Remark: It seems like FIFO page replacement would be trivial to
       implement, just use a circular array index. So further work
       would be implement LRU via a list based stack. Also, in the case of
       a smaller physical memory, we expect the page fault rate to increase and
       TLB-hits to decrease.

# Misc.

    * The given addresses.txt file only uses 244 out of the 256 total frames, so
      without the "Modifications" above, physical memory is never completely
      full.


*/

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include "vm.h"
#include "args.h"
#include "get_vaddrs.h"
#include "backing_store.h"
#include "tlb.h"
#include "pmem.h"
#include "pg_table.h"

int translate_all(void);

/*! @discussion Statistics. */
static size_t nrefs; // Total number of address references.

/*!

    @function main

    @discussion To run the program, use `./a.out addresses.txt`.

    @param argc The usual main() arg.
    @param argv The usual main() arg.

    @result 0 if successful.
*/
int main (int argc, const char * const * const argv) {
    const char *fname = NULL;
    double frac_pf, frac_tlb_h;

    // Process command line args.
    if (valid_int_sizes()) {
      return 1;
    }

    if (valid_args (argc, argv)) {
      return 1;
    }

    if (get_arg_vaddrs_filename (argc, argv, &fname)) {
        return 2;
    }

    printf("Input \"addresses\" filename is: \"%s\"\n", fname);

    printf("OK...\n");

    // Get all virtual addresses
    if (init_vaddrs(fname)) {
        return 3;
    }

    // Translate all virtual addresses
    if (translate_all()) {
        return 4;
    }

    // Print statistics
    frac_pf = ((double)(npf * 100))/((double)nrefs);
    frac_tlb_h = ((double)(ntlb_hits * 100))/((double)nrefs);
    printf("Done. Statistics:\n N REFS %lu\n N PAGE FAULTS %lu (%%%f)\n N TLB "\
           "HITS %lu (%%%f)\n", nrefs, npf, frac_pf, ntlb_hits, frac_tlb_h);

    return 0;
}

/*
    @function translate_all
    @discussion Translates all virtual addresses to physical addresses and
    prints the result.
    @result 0 if successful.
*/
int translate_all(void) {
    size_t i;
    addr_t paddr;
    signed char v;

    for (i = 0; i < vaddrs_len; i++) {
        // Translate from virtual to physical address.

        if(translate_v2p_addr(vaddrs[i], &paddr)) {
            assert(0);
            return 1;
        }

        // Read the value stored at the physical address.
        if(p_mem_read_byte(paddr, &v)) {
            assert(0);
            return 2;
        }

        /* Print result in specified format
           e.g.: "Virtual address: 16916 Physical address: 20 Value: 0" */
        printf("Virtual address: %lu Physical address: %lu Value: %d\n",
               vaddrs[i], paddr, v);
        nrefs++; // Statistics
    }

    free_page_list();
#ifndef NO_TLB
    free_tlb_list();
#endif

    return 0;
}

/*

# Observed Statistics

* No TLB, 256 frames, 256 pages:
  * N REFS 1000
  * N PAGE FAULTS 244 (%24.400000)

* With TLB, 256 frames, 256 pages:
  * N REFS 1000
  * N PAGE FAULTS 244 (%24.400000)
  * N TLB HITS 55 (%5.500000)


* With TLB, 128 frames, 256 pages:
  * N REFS 1000
  * N PAGE FAULTS 538 (%53.800000)
  * N TLB HITS 53 (%5.300000)

Output in all cases matches correct.txt. Verified using `cmp` command.

*/