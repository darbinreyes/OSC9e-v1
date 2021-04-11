#ifndef __BACKING_STORE_H__
#define __BACKING_STORE_H__

#include "vm.h"

int backing_store_pg_in(addr_t page_num, unsigned char *dst);

#endif