#ifndef __LIST_H__
#define __LIST_H__
typedef struct _list_node_t {
    int e;
    struct _list_node_t *next;
} list_node_t;

typedef struct _list_t {
    list_node_t *head;
    list_node_t *tail;
    int len;
} list_t;

int alloc_list(list_t *l, int len);

int free_list(list_t *l);

int free_list2(list_t *l);

int init_list(list_t *l, int *e, int elen);

int list_contains(list_t *l, int e);

int list_remove_node(list_t *l, int e, list_node_t **rm_n);

int list_add_tail(list_t *l, list_node_t *n);

int list_add_head(list_t *l, list_node_t *n);

int print_list(list_t *l);

int test_list(void);

#endif