#ifndef __GET_VADDRS_H__
#define __GET_VADDRS_H__

/*!
    @defined MAX_NUM_V_ADDRS
    @discussion The max number of virtual addresses allowed in the addresses.txt
    file.
*/
#define MAX_NUM_V_ADDRS 1000U

extern addr_t vaddrs[MAX_NUM_V_ADDRS];

extern size_t vaddrs_len;

int init_vaddrs(const char *vaddrs_fname);

#endif