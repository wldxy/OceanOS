#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "type.h"
#include "trap.h"

#define MEMORY_MAX_SIZE 0x20000000

#define MEMORY_PAGE_SIZE 0x1000

#define STACK_SIZE 8192

#define PHY_PAGE_MASK 0xFFFFF000

#define PAGE_MAX_SIZE (MEMORY_MAX_SIZE / MEMORY_PAGE_SIZE)

#define PAGE_OFFSET 0xC0000000

#define PAGE_SIZE 4096

#define PAGE_PRESENT 0x1

#define PAGE_WRITE 0x2

#define PAGE_USER 0x4

#define PAGE_MASK 0xFFFFF000

#define PGD_SIZE (PAGE_SIZE / sizeof(pgd_t))

#define PTE_SIZE (PAGE_SIZE / sizeof(pte_t))

#define PTE_COUNT 128

#define OFFSET_INDEX(x) ((x) & 0xFFF)

#define PTE_INDEX(x) (((x) >> 12) & 0x3FF)

#define PGD_INDEX(x) (((x) >> 22) & 0x3FF)

extern uint8_t _start[];
extern uint8_t _end[];

extern uint32_t pgd_kern[PGD_SIZE];

void init_page();
uint32_t alloc_page();
void free_page();

void init_vmm();
void switch_pgd(uint32_t p);
void map(pgd_t* pgd_now, uint32_t va, uint32_t pa, uint32_t flags);
void unmap(pgd_t *pgd_now, uint32_t va);
uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa);
void page_fault(register_t *reg);

#endif
