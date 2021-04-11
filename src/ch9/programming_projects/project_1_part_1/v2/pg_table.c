/*!
    @header Page table related definitions.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "backing_store.h"
#include "vm.h"
#include "pmem.h"
#include "list.h"
#include "tlb.h"

size_t npf; // @npf Total number of page faults.

/*!
    @typedef pg_list_t
    @discussion Represents a node in the list that is used to implement
    page replacement. Each node points to a valid page table entry which can be
    used to determine the associated page number and frame number.
    @field pg_tbl_entry The associated page table entry.
    @field list Contains pointers to the next and previous nodes in the list.
*/
typedef struct _pg_list_t {
    pg_tbl_entry_t *pg_tbl_entry;
    struct list_head list;
} pg_list_t;

/* @page_list Handle to the list of pages currently in memory. @page_list.next
points to the head of the list, @page_list.prev points to the tail of the list.
The list is maintained in FIFO order such that the head is the first-in page. */
static LIST_HEAD(page_list);

/* @page_table Represents the page table. */
static pg_tbl_entry_t page_table[PAGE_TABLE_LEN];

/*!
    @function page_table_rm

    @discussion Updates the page table to reflect that a particular page is no
    longer in memory because a page replacement took place. Returns the frame
    number originally stored in the entry.

    @param page_num The page table entry to invalidate.
    @param frame_num The frame number originally stored in the page table entry,
    if successful. Otherwise untouched.

    @result 0 if successful.
*/
int page_table_rm(addr_t page_num, addr_t *frame_num) {

    if (page_num >= PAGE_TABLE_LEN) {
        assert(0);
        return 1;
    }

    if (frame_num == NULL) {
        assert(0);
        return 1;
    }

    if(!page_table[page_num].valid) {
        /* We should only be removing valid page table entries. All entries in
           the list should be valid. */
        assert(0);
        return 1;
    }

    page_table[page_num].valid = 0; // No longer in memory.

    *frame_num = page_table[page_num].fn;

    /* Mark with an invalid page number. */
    page_table[page_num].fn = PAGE_TABLE_LEN;

#ifndef NO_TLB
    if (tlb_rm(page_num, *frame_num)) {
        assert(0);
        return 1;
    }
#endif

    return 0;
}

/*!
    @function fifo_list_add

    @discussion Adds a list node to the tail of the FIFO list used to implement
    page replacement. The node is initialized to point to the provided page
    table entry @pte.
    Memory for the node is obtained from malloc(), the caller is responsible for
    calling free().
    Remark: We never need to remove an entry from the list. During page
    replacement we reuse the node at the front of the list and move it to the
    back of the list.

    @param pte Pointer to a page table entry.

    @result 0 if successful.
*/
static int fifo_list_add(pg_tbl_entry_t *pte) {
    pg_list_t *t;

    if (pte == NULL) {
        assert(0);
        return 1;
    }

    t = malloc(sizeof(*t));

    if (t == NULL) {
        assert(0);
        return 1;
    }

    t->pg_tbl_entry = pte;
    INIT_LIST_HEAD(&t->list);
    list_add_tail(&t->list, &page_list);
    return 0;
}

/*!
    @function page_table_add

    @discussion Updates the page table to reflect that a page is now in memory.
    @page_num is the page number that identifies the page table entry that will
    be updated. @frame_num is the frame number in which the page resides, it
    will be saved in the identified page table entry. Finally, the page table
    entry is marked as valid.

    @param page_num A page number, this identifies the page table entry being
    updated.

    @param frame_num A frame number to save in the page table.

    @result 0 if successful.
*/
int page_table_add(addr_t page_num, addr_t frame_num) {
    if (page_num >= PAGE_TABLE_LEN) {
        assert(0);
        return 1;
    }

    if (frame_num >= NUM_PAGE_FRAMES) {
        assert(0);
        return 1;
    }

    if(page_table[page_num].valid) {
        assert(0);
        return 1;
    }

    page_table[page_num].fn = frame_num;
    page_table[page_num].valid = 1;

    return 0;
}

/*!
    @function page_replace

    @discussion When a page fault occurs and there are no free frames, a page
    must be replaced. This function performs a page replacement.

    @param page_num The page number of the page that generated the page fault.

    @param frame_num The frame number in which the requested page has been
    placed, if successful. Otherwise untouched.

    @result 0 if successful.
*/
int page_replace(addr_t page_num, addr_t *frame_num) {
    addr_t victim_frame;
    pg_list_t *t;

    if (page_num >= PAGE_TABLE_LEN) {
        assert(0);
        return 1;
    }

    if (frame_num == NULL) {
        assert(0);
        return 1;
    }

    if (list_empty(&page_list)) {
        /* We are performing page replacement, the list should not be empty, the
           list should contain as many nodes as there are page frames. */
        assert(0);
        return 1;
    }

    /* FIFO page replacement: the victim page is always the page at the
       head of the list. */
    t = list_first_entry(&page_list, pg_list_t, list);

    if(page_table_rm(t->pg_tbl_entry - page_table, /* victim's page number */
                     &victim_frame)) {
        assert(0);
        return 1;
    }

    if(backing_store_pg_in(page_num, p_mem_addr() + FRAME_NUM_ADDR(victim_frame))) {
      assert(0);
      return 3;
    }

    if(page_table_add(page_num, victim_frame)) {
        assert(0);
        return 4;
    }

    /* Update page table entry pointer of the list node to point to the entry of
     the page that was paged in. */
    t->pg_tbl_entry = &page_table[page_num];

    /* Move the list entry from the head to the tail of the queue. This page is
    now the page most recently brought into memory. */
    list_move_tail(&t->list, &page_list);

    *frame_num = victim_frame;

    return 0;
}

/*!
    @function translate_v2p_addr

    @discussion Translates the given virtual address to a physical address.

    @param vaddr The virtual address.
    @param paddr The physical address, if successful.

    @result 0 if successful.
*/
int translate_v2p_addr(addr_t vaddr, addr_t *paddr) {
    /* @free_frame: The page frame number of the next free frame. Initially, all
       page faults are serviced by inserting the page into a frame number equal
       to @free_frame. If @free_frame equals @NUM_PAGE_FRAMES, there are no more
       free frames and all subsequent page faults are serviced by page
       replacement. */
    static addr_t free_frame = 0;
    addr_t page_offset, page_num, frame_num;

    if (vaddr > MAX_V_ADDR) {
        assert(0);
        return 1;
    }

    if (paddr == NULL) {
        assert(0);
        return 1;
    }

    /*
        Steps:
        Extract the page offset and page number from the virtual address.

        Consult the TLB for the translation. On a TLB hit, take the frame number
        in the TLB entry, and concatenate that frame number and page offset to
        form the physical address. Otherwise, a TLB miss has occurred, proceed
        as follows.

        Take the page number as an index into the page table to obtain the page
        table entry.

        If the page table entry indicates that the page is in memory, save the
        frame number from that page table entry. Concatenate the frame number
        and page offset to form the physical address.

        Otherwise a page fault has occurred, i.e. the page is not in memory.

        If a page fault occurs and there is a free frame, read the page from the
        backing store into a free frame, and update the page table. Concatenate
        the free frame number and page offset to form the physical address.

        If a page fault occurs and there are no free frames, replace a page
        currently in memory with the page that generated the page fault.
        Concatenate the frame number of the new page and page offset to form the
        physical address.
    */

    page_offset = ADDR_PAGE_OFFSET(vaddr);
    page_num = ADDR_PAGE_NUM(vaddr);

    if (page_num >= PAGE_TABLE_LEN) {
        assert(0);
        return 1;
    }

#ifndef NO_TLB
    if (in_tlb(page_num, &frame_num)) {
        // Page is in memory. Translation resolved in the TLB.
        *paddr = PHYSICAL_ADDR(frame_num, page_offset);
        return 0;
    }
#endif

    if (page_table[page_num].valid) {
        // The page is in memory.
        *paddr = PHYSICAL_ADDR(page_table[page_num].fn, page_offset);
        return 0;
    }

    /* Page fault! Must get the page from the backing store. */

    npf++; // Statistics

    if (free_frame < NUM_PAGE_FRAMES) {
        // Use a free frame.
        frame_num = free_frame++;
        if(backing_store_pg_in(page_num, p_mem_addr() + FRAME_NUM_ADDR(frame_num))) {
          assert(0);
          return 1;
        }

        if(page_table_add(page_num, frame_num)) {
          assert(0);
          return 1;
        }

        if (fifo_list_add(&page_table[page_num])){
            assert(0);
            return 1;
        }
    } else {
        // No free frame available, page replacement required.
        if (page_replace(page_num, &frame_num)) {
            assert(0);
            return 1;
        }
    }

#ifndef NO_TLB
    if (tlb_add(page_num, frame_num)) {
        assert(0);
        return 1;
    }
#endif

    // pg_list_t *pos;

    // list_for_each_entry(pos, &page_list, list) {
    //     printf("[%lu|%lu]->", (pos->pg_tbl_entry - page_table), pos->pg_tbl_entry->fn);
    // }

    // printf("\n");

    *paddr = PHYSICAL_ADDR(frame_num, page_offset);
    return 0;
}

/*!
    @discussion Frees all dynamically allocated memory used. To be called only
    when all address translations have been performed.
*/
void free_page_list(void) {
    pg_list_t *pos, *n;

    if (list_empty(&page_list))
        return;

     list_for_each_entry_safe(pos, n, &page_list, list) {
         list_del(&pos->list);
         free(pos);
     }
}