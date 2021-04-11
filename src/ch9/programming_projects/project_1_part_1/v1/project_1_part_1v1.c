/*!

    @header Chapter 9 Programming Project: Designing a Virtual Memory Manager
    @discussion
    * This implementation started with `v0/project_1_part_1v0.c`
      and was extended by adding a TLB, statistics, and finally reducing the
      size of physical memory. See `P_MEM_SIZE` in `vm.h`.
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

int translate_v2p_addr(addr_t vaddr, addr_t *paddr);
int p_mem_read(addr_t paddr, signed char *v);
int translate_all(void);

/*! @discussion Statistics. */
static size_t nrefs; // Total number of address references.
static size_t npf; // Total number of page faults.
static size_t ntlb_hits; // Total number of TLB-hits.

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

    // Translate all virtual addressese
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
        if(p_mem_read(paddr, &v)) {
            assert(0);
            return 2;
        }

        /* Print result in specified format
           e.g.: "Virtual address: 16916 Physical address: 20 Value: 0" */
        printf("Virtual address: %lu Physical address: %lu Value: %d\n",
               vaddrs[i], paddr, v);
        nrefs++; // Statistics
    }

    return 0;
}

/*! @discussion Represents the TLB. */
static tlb_entry_t tlb[TLB_LEN];

/*!
    @function in_tlb
    @discussion Consults the TLB for a translation.

    @param page_num The page number.

    @param frame_num On a TLB-hit, the corresponding frame number. On a
    TLB-miss, untouched.

    @result 1 on a TLB-hit, 0 on a TLB-miss.
*/
int in_tlb(addr_t page_num, addr_t *frame_num) {
    size_t i;

    if (frame_num == NULL) {
        assert(0);
        return 0;
    }

    for (i = 0; i < TLB_LEN; i++) {
        if (tlb[i].valid && tlb[i].pn == page_num) {
            //printf("TLB-hit!\n");
            *frame_num = tlb[i].fn;
            return 1;
        }
    }
    //printf("TLB-miss!\n");
    return 0;
}

/*!
    @function tlb_add
    @discussion Updates the TLB with a new translation entry - for use after a
    page fault occurs. Entries are replaced according to FIFO.

    @param page_num The page number.

    @param frame_num The frame number.

    @result 0 if successful.
*/
int tlb_add(addr_t page_num, addr_t frame_num) {
    /* Implements FIFO TLB entry replacement using a circular array index. */

    /* @IMPORTANT Since we are not required to replace any pages, once the
    TLB is full, all entries remain valid, we only need to update the entry's
    page number and frame number. @TODO This is no longer true once we are
    evicting pages from memory and thus TLB entries are being invalidated. We
    only want to replace an entry if the TLB is full.
    TLB_LEN = 3. victim = 0.

    []<
    []
    []

    victim = 0. (page_num, frame_num)=[2|0]

    [2|0] // State of TLB after call.
    []< // victim index
    []

    victim = 1. (page_num, frame_num)=[5|1]

    [2|0]
    [5|1]
    []<

    victim = 2. (page_num, frame_num)=[7|2]

    [2|0]<
    [5|1]
    [7|2]

    // At this point the next call will replace entry 0. Which is correct
    according to FIFO replacement.

    victim = 0. (page_num, frame_num)=[1|3]

    [1|3] // replaced entry
    [5|1]<
    [7|2]

    // Do we need to check whether or not a page is already in the TLB before
    an add? We only add on a page fault, we only invalidate when a page is
    evicted, we only replace when the TLB is full and an add occurs. The assert
    below performs this sanity check.

    It seems that now we have a queue in which items can be removed from places
    other than the head. In this case an array based queue will not suffice.

    */
    static size_t victim = 0;
    addr_t t;
    size_t i;

    assert(!in_tlb(page_num, &t)); // Sanity check.

    for (i = 0; i < TLB_LEN; i++) {
        if (!tlb[i].valid) {
            printf("TLB not full\n");
            break;
        }
    }

    tlb[victim].pn = page_num;
    tlb[victim].fn = frame_num;

    tlb[victim].valid = 1;
    victim++;
    victim = victim % TLB_LEN;
    return 0;
}

/*!
    @function tlb_rm

    @discussion Removes the specified entry from the TLB. This operation is
    necessary any time we evict a page from memory.

    @param page_num The page number.

    @param frame_num The frame number.

    @result 0 if successful.
*/
int tlb_rm(addr_t page_num, addr_t frame_num) {
    size_t i;

    // Remark: TLB's are small, so in most cases we won't remove any entry.

    //printf("TLB entry rm.\n");

    for (i = 0; i < TLB_LEN; i++) {
        if (tlb[i].valid && tlb[i].pn == page_num && tlb[i].fn == frame_num) {
            // Remove entry.
            tlb[i].valid = 0;
            /* PAGE_TABLE_LEN is not a valid frame or page number, so it is a
               good value to use for marking entries as removed. */
            tlb[i].fn = PAGE_TABLE_LEN;
            tlb[i].pn = PAGE_TABLE_LEN;
            /* @IMPORTANT Possible bug. I never see this print. This implies
            that each page that is evicted from memory by chance never happens
            to have an entry in TLB. */
            printf("TLB entry removed.\n");
            break;
        }
    }

    return 0;
}

/*!
    @function evict_page
    @discussion When a page fault occurs and there are no free frames a page
    must be evicted from memory in order to bring in a new page. This function
    first selects a page to evict from memory and then writes it to the backing
    store. The eviction results in a free frame, the frame number of the freed
    frame is returned if successful.

    @param free_frame The freed frame if successful, otherwise untouched.

    @result 0 if successful.
*/
int evict_page(addr_t *free_frame) {
  static addr_t victim_pg = 0;

  /* FIFO replacement is trivial to implement for this project, just use
  a circular array index, the index = the frame to be replaced.
  @IMPORTANT FIFO page replacement in this function depends on the page
  frames being filled from lowest frame number (0) to highest frame number
  (e.g. 127). Only when there are no free frames should this function be called.
  Thus, once the 127th frame is filled, the next memory reference will page
  fault, we will see that memory is full, at which point this function will be
  called, and it will return free_frame = 0, which is indeed the FIFO page. The
  next call will return free_frame = 1, etc.
  @TODO We can verify the correctness of the above remark by implementing FIFO
  with a linked list. It should be too hard since most of the work will be done
  once the TLB is fixed.
  */

  if (free_frame == NULL) {
    assert(0);
    return 1;
  }

  *free_frame = (victim_pg++ % NUM_PAGE_FRAMES);

  //printf("Evicted page frame number %lu\n", *free_frame);

  return 0;
}

/*! @discussion Represents physical memory. */
static unsigned char p_mem[P_MEM_SIZE];

/*
    @discussion @TODO @NEXT
    Convincing myself I have not introduced a bug/error.
    The fact that the TLB hit rate did not decrease, nor change at all
    may be OK.

    Suppose the random addresses given are such that FIFO replacement
    of pages and TLB entries happens to be ideal. Once there are no
    more free frames, the pages being evicted will be those pages
    brought into memory first and the TLB will be filled with entries
    that page faulted most recently. FIFO isn’t LRU, but we can still
    meaningfully talk about how long certain pages have resided in
    memory. The U in LRU is for USED  (read/written) not how long the
    page has been in memory. The page at the the front of the queue,
    the first in, is also the page that has resided in memory longest.

    So as long as the TLB is full of entries that have been in memory the least
    amount of time and we are evicting pages by FIFO, those evicted pages are the
    ones residing in memory for the longest amount of time. These pages are unlikely
    to overlap. Thus it is not completely out of the realm of possibility that the
    TLB hit rate did not change after accounting for TLB entry removal during page
    eviction. Print tlb state w/ either printf or lldb.
*/

/*!
    @function translate_v2p_addr

    @discussion Translates the given virtual address to a physical address.

    @param vaddr The virtual address.
    @param paddr The physical address if successful.

    @result 0 if successful. Otherwise error.
*/
int translate_v2p_addr(addr_t vaddr, addr_t *paddr) {
    /*! @discussion The page frame number of the next free frame. When a page
        fault occurs, the new page is read into physical memory at byte address
        == free_framen * PAGE_SIZE. Unless there are no free frames, in which
        case the new page is read into memory after evicting a page to make
        room. */
    static addr_t free_framen = 0;
    /* Represents the page table. */
    static pg_tbl_entry_t page_table[PAGE_TABLE_LEN];
    addr_t page_offset, page_num, frame_num;
    size_t i;

    /*
        Steps:
        Extract the page offset and page number from the virtual address.

        Take the page number as an index into the page table to obtain the page
        table entry.

        If the page table entry indicates that the page is in memory, save the
        frame number from that page table entry. Concatenate the frame number
        and page offset to form the physical address.

        Otherwise a page fault has occurred, i.e. the page is not in memory.

        If a page fault occurs and there is a free frame, read the page from the
        backing store into a free frame, update the page table, and add a
        corresponding TLB entry to the TLB. Concatenate the frame number and
        page offset to form the physical address.

        If a page fault occurs and there are no free frames, evict a page from
        memory, read the page from the backing store into the newly freed frame,
        update the page table, and add a corresponding TLB entry to the TLB.
        Concatenate the frame number and page offset to form the physical
        address.
    */

    if (paddr == NULL) {
        assert(0);
        return 1;
    }

    page_offset = (vaddr & (PAGE_SIZE - 1));
    page_num = (vaddr & (V_MEM_SIZE - 1)) >> PAGE_OFFSET_NBITS;

    if (page_num >= PAGE_TABLE_LEN) {
        assert(0);
        return 1;
    }

    if (in_tlb(page_num, &frame_num)) {
        ntlb_hits++; // Statistics. TLB-hit
    } else if (page_table[page_num].im) {
        // The page is in memory.
        frame_num = page_table[page_num].fn;
    } else {
        /* Page fault! Get the page from the backing store, update page_table
        and TLB. */
        npf++; // Statistics
        /*! @discussion
          * Supporting physical memory smaller than virtual memory.
          * Reduce size of physical mem. from 256 to 128 frames.
          * if free_framen == NUM_PAGE_FRAMES
              * no more free page frames
              * a page must be evicted from memory to backing store
              * since we are never writing to a page we can skip writing the
                page from memory to the backing store.

              * all we need to do is decide which page will be be replaced.
              * FIFO replacement can be implemented using a circular array index.
              * Once memory is full, page replacement will be determined
                according to FIFO.
              * After evicting a page, we have a free frame, and we proceed with
                reading the into memory from the backing store and updating the
                page table and TLB.
        */
        if (free_framen >= NUM_PAGE_FRAMES) {
          // No free frame available, page replacement required.
          //printf("Memory is full!\n");
          if(evict_page(&frame_num)) {
            assert(0);
            return 1;
          }

          /*!
              # Results

              v0 statistics
              N REFS 1000
              N PAGE FAULTS 244 (%24.400000)
              N TLB HITS 55 (%5.500000)

              v1 statistics
              N REFS 1000
              N PAGE FAULTS 538 (%53.800000)
              N TLB HITS 53 (%5.300000)

              correct.txt
              Virtual address: 48855 Physical address: 32983 Value: -75
              v0 correct.txt
              Virtual address: 48855 Physical address: 32983 Value: -75
              v1 correct.txt
              Virtual address: 48855 Physical address: 215 Value: -75

              In v1, we expect a difference in at at least part of the physical
              addresses since we have reduced the number of page frames.
          */

          for (i = 0; i < PAGE_TABLE_LEN; i++) {
              /*!
                  @discussion The page being evicted is the one residing in page
                  frame number frame_num indicated by evict_page(). Since the page
                  currently resides in memory there is a corresponding page table
                  entry marked as valid. Find that page table entry and mark it as
                  invalid.
              */
              if (page_table[i].im && page_table[i].fn == frame_num) {
                  page_table[i].im = 0; // No longer in memory.
                  /* Mark with an invalid page number. */
                  page_table[i].fn = PAGE_TABLE_LEN;
                  break;
              }
          }

          if (i >= PAGE_TABLE_LEN || tlb_rm(i, frame_num)) {
              /*! @discussion Unless there is an bug, the for loop above should
              always be terminated by the break statement, not the test part of
              the for loop, since by definition a page being evicted must
              currently reside in memory, and hence have a corresponding valid
              entry in the page table. If this page has an entry in the TLB
              then tlb_rm() invalidates that entry. */
              assert(0);
              return 1;
          }

        } else {
          // Use a free frame.
          frame_num = free_framen;
        }

        if (frame_num >= NUM_PAGE_FRAMES) {
            fprintf(stderr, "frame_num is out of bounds %lu >= %lu.\n", \
                    frame_num, NUM_PAGE_FRAMES);
            assert(0);
            return 2;
        }

        if(backing_store_read(page_num, p_mem + frame_num * PAGE_SIZE)) {
            assert(0);
            return 3;
        }

        if (tlb_add(page_num, frame_num)) {
            assert(0);
            return 1;
        }

        page_table[page_num].fn = frame_num;
        page_table[page_num].im = 1;
        if (free_framen < NUM_PAGE_FRAMES)
          free_framen++;
    }

    assert(frame_num < NUM_PAGE_FRAMES);

    *paddr = (frame_num << PAGE_OFFSET_NBITS) | page_offset;
    return 0;
}

/*!
    @function p_mem_read

    @discussion Returns the value of a byte in physical memory at the given
    physical address.

    @param paddr The physical address.

    @param v If successful, the value of the byte at the address.

    @result 0 if successful.
*/
int p_mem_read(addr_t paddr, signed char *v) {
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
