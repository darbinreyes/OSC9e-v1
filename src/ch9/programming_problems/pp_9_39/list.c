#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int alloc_list(list_t *l, int len) {
    list_node_t *tn;

    assert(l != NULL && len >= 0);

    if (l == NULL || len < 0) {
        return 1;
    }

    l->len = len;

    if(len == 0) { // Empty list.
        l->head = NULL;
        l->tail = NULL;
        return 0;
    }

    for (int i = 0; i < len; i++) {
        if (i == 0) {
            l->head = calloc(1, sizeof(list_node_t));
            if (l->head == NULL)
                return 2;
            tn = l->head;
        } else {
            tn->next = calloc(1, sizeof(list_node_t));
            if (tn->next == NULL)
                return 2;
            tn = tn->next;
        }
    }

    l->tail = tn;

    return 0;
}

int free_list(list_t *l) {

    assert(l != NULL);

    if (l == NULL) {
        return 1;
    }

    if (l->head == NULL)
        return 0; // Empty list.

    for(list_node_t *tn = l->head, *tn2 = l->head->next; tn != NULL; ) {
        free(tn);
        tn = tn2;
        if (tn2 != NULL)
            tn2 = tn2->next;
    }

    return 0;
}


int free_list2(list_t *l) {
    list_node_t *n0, *n1;

    assert(l != NULL);

    if (l == NULL) {
        return 1;
    }

    if (l->head == NULL)
        return 0; // Empty list.

    n0 = l->head;
    n1 = n0->next;

    while(n0 != NULL) {
        free(n0);

        n0 = n1;

        if(n1 != NULL) {
            n1 = n1->next;
        }
    }

    return 0;
}

int init_list(list_t *l, int *e, int elen) {
    int i;
    list_node_t *n;
    // head = stack bottom = LRU page. = e[0].
    // tail = stack top = MRU page. = e[elen-1].

    assert(l != NULL && e != NULL && elen > 0 && l->len == elen);

    if (l == NULL || e == NULL || elen <= 0 || l->len != elen) {
        return 1;
    }

    for(i = 0, n = l->head; i < elen && n != NULL; i++, n = n->next) {
        n->e = e[i];
    }

    return 0;
}

int list_contains(list_t *l, int e) {
    // If e occurs more than once, the search terminates when the first occurrence is found.
    list_node_t *n;


    assert(l != NULL);

    if(l == NULL) {
        return -1;
    }

    if(l->len == 0) { // Empty list.
        return 0;
    }

    n = l->head;

    while (n != NULL) {
        if(n->e == e)
            return 1; // found!
        n = n->next;
    }

    return 0; // Not in list!
}

int list_remove_node(list_t *l, int e, list_node_t **rm_n) {
    // Searches for the first node with entry e, removes it from the list if found, the removed node is returned to the called in *rm_n. If not found *rm_n = NULL.
    // Returns 0 if the e is not in the list. Returns 1 if e is found. Other values indicate an error occurred.
    list_node_t *b, *n, *a;

    assert(l != NULL && rm_n != NULL);

    if (l == NULL || rm_n == NULL) {
        return -1;
    }

    if(l->len == 0) { // Empty list.
        *rm_n = NULL;
        return 0;
    }

    /*
        3 cases:
            Head removal
            Tail removal
            Middle removal
    */

    b = NULL;
    n = l->head;

    while(n != NULL) {
        if(n->e == e) {
            break;
        }

        b = n;
        n = n->next;
    }

    if(n == NULL) { // e is not in the list.
        *rm_n = NULL;
        return 0;
    }

    *rm_n = n; // Return removed node.
    l->len--; // Removing 1 node.

    if(l->len == 0) { // Removing the only node in the list.
        assert(l->head == l->tail);
        l->head = NULL;
        l->tail = NULL;
        return 1;
    }

    a = n->next; // node after the node being removed.

    if(n == l->head) { // head removal
        l->head = a; // new head is the node after the current head.
        n->next = NULL; // unlink node being removed.
        return 1;
    }

    if(n == l->tail) { // tail removal
        l->tail = b; // new tail is node before the node being removed.
        l->tail->next = NULL; // Tail next must be marked NULL.
        return 1;
    }

    // middle removal
    assert(b != NULL && a != NULL);
    b->next = a;
    n->next = NULL;

    return 1;

//  l->len-- ; if found e and removed it. ++ inside list_add_head/tail/at().
}

int list_add_tail(list_t *l, list_node_t *n) {
    assert(l != NULL && n != NULL);

    if(l == NULL || n == NULL) {
        return -1;
    }

    n->next = NULL; // Tail next must be marked NULL.
    l->len++;
    if(l->len == 1) { // first node being added to the list
        l->head = n;
        l->tail = n;
        return 0;
    }

    l->tail->next = n;
    l->tail = n;

    return 0;
}

int list_add_head(list_t *l, list_node_t *n) {
    assert(l != NULL && n != NULL);

    if(l == NULL || n == NULL) {
        return -1;
    }

    l->len++;

    if(l->len == 1) { // first node being added to the list
        l->head = n;
        l->tail = n;
        n->next = NULL; // Tail next must be marked NULL.
        return 0;
    }

    n->next = l->head;
    l->head = n;
    return 0;
}

int print_list(list_t *l) {
    list_node_t *n;

    if(l == NULL) {
        printf("l NULL!\n");
        return 1;
    }

    if(l->head == NULL || l->len == 0){
        printf("list empty!\n");
        return 2;
    }

    n = l->head;

    while(n != NULL) {
        printf("%d->", n->e);
        n = n->next;
    }

    printf("\n");

    return 0;
}

int test_list(void) {
    int a[] = {2, 3, 5};
    int len = sizeof(a)/sizeof(a[0]);
    list_t l;
    list_node_t *rm_n = NULL;

    if (alloc_list(&l, len)) {
        printf("alloc_list() error.\n");
        return 1;
    }

    if(init_list(&l, a, len)) {
        printf("init_list() error.\n");
        return 2;
    }

    print_list(&l);

    if(list_remove_node(&l, 3, &rm_n) == 1){
        printf("Remove node %d\n", rm_n->e);
        if(list_add_tail(&l, rm_n) == 0)
            printf("Added tail\n");
    }

    print_list(&l);

    free_list2(&l);

    return 0;
}