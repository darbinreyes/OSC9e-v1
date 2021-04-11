/*

This is my solution to chapter 8 programming problem 8.33 from Operating System
Concepts,  Galvin.

Example:

`./a.out 19986`
```
The address 19986 contains:
page number = 4
offset = 3602
```

*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    unsigned long vaddr;
    unsigned long page_number;
    unsigned long page_offset;

    if (argc != 2) {
        printf("Usage example: ./a.out 19986. Values beyond 32-bits will be truncated to 32-bits.\n");
        return 1;
    }

    // 4 KB page = 2^12 bytes // 12 bits for page offset // 32 - 12 = 20 bits page number.
    // (int)strtol(str, (char **)NULL, 10);
    vaddr = strtoul(argv[1], (char **)NULL, 10) & 0x0FFFFFFFF;

    page_number = vaddr >> 12;
    page_offset = vaddr & 0x0FFF;
    printf("The address %lu contains:\n", vaddr);
    printf("page number = %lu\n", page_number);
    printf("page_offset = %lu\n", page_offset);

    return 0;
}