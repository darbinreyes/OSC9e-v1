#ifndef __SORT_H__
#define __SORT_H__

void sort(int a[], unsigned int len);
void print_a(int a[], unsigned int len);
void merge(int am[], const unsigned int am_len, \
           int a0[], const unsigned int a0_len, \
           int a1[], const unsigned int a1_len);
#endif