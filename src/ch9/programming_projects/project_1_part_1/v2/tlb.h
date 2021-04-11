#ifndef __TLB_H__
#define __TLB_H__

int in_tlb(addr_t page_num, addr_t *frame_num);
int tlb_add(addr_t page_num, addr_t frame_num);
int tlb_rm(addr_t page_num, addr_t frame_num);
void free_tlb_list(void);

extern size_t ntlb_hits;

#endif