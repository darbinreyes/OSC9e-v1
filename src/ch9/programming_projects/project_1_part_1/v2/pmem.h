#ifndef __PMEM_H__
#define __PMEM_H__

int p_mem_read_byte(addr_t paddr, signed char *v);
unsigned char * const p_mem_addr(void);

#endif