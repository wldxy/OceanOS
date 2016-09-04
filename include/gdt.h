#ifndef _GDT_H_
#define _GDT_H_

#include "type.h"

typedef
struct gdt_t {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed)) gdt_t;

typedef
struct gdtr_t {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdtr_t;

void init_gdt();

extern void gdt_to_gdtr();

#endif
