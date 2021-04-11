/*!
    @header TLB related definitions.
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "vm.h"
#include "list.h"

size_t ntlb_hits; // Total number of TLB-hits.

/*!
    @typedef tlb_list_t
    @discussion List node for FIFO list used to implement TLB entry replacement.
    @field tlb_entry Pointer to the corresponding TLB entry.
    @field list Contains pointers to previous and next nodes in the list.
*/
typedef struct _tlb_list_t {
    tlb_entry_t *tlb_entry;
    struct list_head list;
} tlb_list_t;

/* @tlb_list Handle to list of TLB entries in FIFO order. Used to implement FIFO
   TLB entry replacement. */
static LIST_HEAD(tlb_list);

/* @tlb Represents the TLB. */
static tlb_entry_t tlb[TLB_LEN];

/* @tlb_entry_count The number of valid entries currently in the TLB. */
static size_t tlb_entry_count = 0;

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

    if (page_num >= PAGE_TABLE_LEN) {
        assert(0);
        return 0;
    }

    if (frame_num == NULL) {
        assert(0);
        return 0;
    }

    if (tlb_entry_count == 0) {
        return 0;
    }

    for (i = 0; i < TLB_LEN; i++) {
        if (tlb[i].valid && tlb[i].pn == page_num) {
            *frame_num = tlb[i].fn;
            ntlb_hits++; // Statistics
            return 1;
        }
    }

    return 0;
}

/*!
    @function fifo_list_add
    @discussion Adds an entry to the back of the FIFO list used to implement
    FIFO TLB entry replacement.
    @param tlbe The TLB entry to store in the list node.
    @result 0 if successful.
*/
static int fifo_list_add(tlb_entry_t *tlbe) {
    tlb_list_t *t;

    if (tlbe == NULL) {
        assert(0);
        return 1;
    }

    t = malloc(sizeof(*t));

    if (t == NULL) {
        assert(0);
        return 1;
    }

    t->tlb_entry = tlbe;
    INIT_LIST_HEAD(&t->list);
    list_add_tail(&t->list, &tlb_list);
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
    size_t i;
    tlb_list_t *t;

    if (tlb_entry_count < TLB_LEN) {
        // Use a free TLB entry.
        for (i = 0; i < TLB_LEN; i++) {
            if (!tlb[i].valid) {
                tlb[i].pn = page_num;
                tlb[i].fn = frame_num;
                tlb[i].valid = 1;

                if (fifo_list_add(&tlb[i])) {
                    assert(0);
                    return 1;
                }

                tlb_entry_count++; // Update entry count
                return 0;
            }
        }
        // The TLB is not full, we should have returned above.
        assert(0);
        return 1;
    }

    /* A TLB entry replacement is required to add a new entry. The number of
       entries remains unchanged. */

    if (list_empty(&tlb_list)) {
        /* We are performing a TLB replacement, the list should not be empty,
           the list should contain as many nodes as there are TLB entries. */
        assert(0);
        return 1;
    }

    // The replaced entry is always at the front of the list.
    t = list_first_entry(&tlb_list, tlb_list_t, list);

    if (!t->tlb_entry->valid) {
        assert(0); // Sanity check. TLB is full. All entries should be valid.
        return 1;
    }

    t->tlb_entry->pn = page_num;
    t->tlb_entry->fn = frame_num;

    // Move the new entry to back of the list.
    list_move_tail(&t->list, &tlb_list);

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

    if (tlb_entry_count == 0) {
        return 0;
    }

    for (i = 0; i < TLB_LEN; i++) {
        if (tlb[i].valid && tlb[i].pn == page_num && tlb[i].fn == frame_num) {
            break;
        }
    }

    if (i >= TLB_LEN) {
        // No matching entry in TLB. Nothing to remove.
        return 0;
    }

    // Remove corresponding FIFO list node.

    /* Note: Although TLB entry replacement is FIFO, a page replacement may
       require that we remove an list entry from the middle of the list. */

    tlb_list_t *pos, *n;

    list_for_each_entry_safe(pos, n, &tlb_list, list) {

        if (pos->tlb_entry == &tlb[i]) {
            list_del(&pos->list);
            free(pos);
            break;
        }
     }

    // Mark entry as invalid.

    tlb[i].valid = 0;
    tlb[i].fn = PAGE_TABLE_LEN;
    tlb[i].pn = PAGE_TABLE_LEN;

    tlb_entry_count--; // Update entry count

    return 0;
}

// @TODO free all in list


/*!
    @discussion Frees all dynamically allocated memory used. To be called only
    when all address translations have been performed.
*/
void free_tlb_list(void) {
    tlb_list_t *pos, *n;

    if (list_empty(&tlb_list))
        return;

    list_for_each_entry_safe(pos, n, &tlb_list, list) {
        list_del(&pos->list);
        free(pos);
    }
}