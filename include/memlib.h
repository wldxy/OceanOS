#ifndef _MEMLIB_H_
#define _MEMLIB_H_

#include "type.h"

#define V_MM_START 0xE0000000

typedef
struct mm_header {
    struct mm_header* prev;
    struct mm_header* next;
    uint32_t used : 1;
    uint32_t length : 31;
} header_t;

void* kmalloc(uint32_t size);

void kfree(void* obj);

#endif // _MEMLIB_H_

