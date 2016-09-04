#include "gdt.h"
#include "string.h"
#include "type.h"

#define GDT_LENGTH 5

gdt_t gdt[GDT_LENGTH];

gdtr_t gdtr;

static void setGdt(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

extern uint32_t stack;

void init_gdt() {
    gdtr.limit = sizeof(gdtr) * GDT_LENGTH - 1;
    gdtr.base = (uint32_t)&gdt;

    setGdt(0, 0, 0, 0, 0);
    setGdt(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    setGdt(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    setGdt(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    setGdt(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    gdt_to_gdtr((uint32_t)&gdtr);
}

static void setGdt(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low     = (base & 0xFFFF);
    gdt[num].base_middle  = (base >> 16) & 0xFF;
    gdt[num].base_high    = (base >> 24) & 0xFF;

    gdt[num].limit_low    = (limit & 0xFFFF);
    gdt[num].granularity  = (limit >> 16) & 0x0F;

    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access       = access;
}
