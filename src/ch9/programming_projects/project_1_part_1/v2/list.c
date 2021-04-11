/*!
    @header Linked list.
    Mostly copied from the Linux open source:
    linux-headers-3.16.0-4-common/include/linux/types.h
    linux-headers-3.16.0-4-common/include/linux/list.h
    linux-headers-3.16.0-4-common/include/linux/kernel.h
    linux-headers-3.16.0-4-common/include/linux/stddef.h
    linux-headers-3.16.0-4-common/include/linux/poison.h
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"

#if 0 // For testing, if needed.

struct bday {
    int year;
    struct list_head list;
};

static LIST_HEAD(pages_list);

int main(void) {
    printf("Hello linux list\n");
    struct bday *t, *t2, *ptr;

    t = malloc(sizeof(*t));

    assert(t);

    t->year = 1988;
    INIT_LIST_HEAD(&t->list);
    list_add_tail(&t->list, &pages_list);

    t2 = malloc(sizeof(*t2));

    assert(t2);

    t2->year = 2000;
    INIT_LIST_HEAD(&t2->list);
    list_add_tail(&t2->list, &pages_list);

    list_for_each_entry(ptr, &pages_list, list) {
        printf("year = %d\n", ptr->year);
    }

    // list_del(&t->list); // remove head
    list_del(pages_list.next); // remove head

    list_for_each_entry(ptr, &pages_list, list) {
        printf("x year = %d\n", ptr->year);
    }

    list_add_tail(&t->list, &pages_list);

    list_for_each_entry(ptr, &pages_list, list) {
        printf("y year = %d\n", ptr->year);
    }
    // @TODO !!! call free()
    return 0;
}
#endif