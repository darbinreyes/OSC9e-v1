#ifndef __PAGE_REPL_H__
#define __PAGE_REPL_H__

int do_lru(const int len_rs, const int * const rs, const int npf);
int do_opt(const int len_rs, const int * const rs, const int npf);
int do_fifo(const int len_rs, const int * const rs, const int npf);

#endif