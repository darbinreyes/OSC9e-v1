#ifndef __BACKING_STORE_H__
#define __BACKING_STORE_H__

int backing_store_read(addr_t page_num, unsigned char *dst);

#endif