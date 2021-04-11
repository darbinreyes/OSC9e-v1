#ifndef __PG_TABLE_H__
#define __PG_TABLE_H__

int translate_v2p_addr(addr_t vaddr, addr_t *paddr);
void free_page_list(void);

extern size_t npf;

#endif