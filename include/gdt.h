#ifndef _GDT_H_
#define _GDT_H_

#include "type.h"

struct gdt_struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));
typedef struct gdt_struct gdt_t;

struct gdtr_struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));
typedef struct gdtr_struct gdtr_t;

void init_gdt();

extern void gdt_to_gdtr();

#endif
