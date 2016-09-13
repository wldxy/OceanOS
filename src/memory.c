#include "type.h"
#include "memory.h"
#include "debug.h"
#include "multiboot.h"
#include "trap.h"
#include "string.h"
#include "system.h"

static uint32_t _stack[PAGE_MAX_SIZE+1];
static uint32_t _top;

uint32_t phy_page_count;

pgd_t pgd_kern[PGD_SIZE] __attribute__ ((aligned(PAGE_SIZE)));
static pte_t pte_kern[PTE_COUNT][PTE_SIZE] __attribute__ ((aligned(PAGE_SIZE)));

void init_page() {
    memory_map_t *map_start_addr = (memory_map_t *)glb_mboot_ptr->mmap_addr;
    memory_map_t *map_end_addr = (memory_map_t *)glb_mboot_ptr->mmap_addr + glb_mboot_ptr->mmap_length;
    memory_map_t *cur_addr;

    for (cur_addr = map_start_addr;cur_addr < map_end_addr;cur_addr++) {
        if (cur_addr->type == 1 && cur_addr->base_addr_low == 0x100000) {
            uint32_t page_addr = cur_addr->base_addr_low + (uint32_t)(_end - _start);
			uint32_t length = cur_addr->base_addr_low + cur_addr->length_low;

			while (page_addr < length && page_addr <= MEMORY_MAX_SIZE) {
				free_page(page_addr);
				page_addr += MEMORY_PAGE_SIZE;
			}
        }
    }
}

uint32_t alloc_page() {
    assert(_top != 0, "out of memory");

    uint32_t page_addr = _stack[_top--];

    return page_addr;
}

void free_page(uint32_t p) {
    assert(_top != PAGE_MAX_SIZE, "no unused memory");

    _stack[++_top] = p;

}

void init_vmm() {
    uint32_t kern_pte_first_idx = PGD_INDEX(PAGE_OFFSET);

    uint32_t i, j;
    for (i = kern_pte_first_idx, j = 0; i < PTE_COUNT + kern_pte_first_idx; i++, j++) {
        pgd_kern[i] = ((uint32_t)pte_kern[j] - PAGE_OFFSET) | PAGE_PRESENT | PAGE_WRITE;
    }

    uint32_t *pte = (uint32_t *)pte_kern;
    for (i = 1; i < PTE_COUNT * PTE_SIZE; i++) {
        pte[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
    }

    uint32_t pgd_kern_phy_addr = (uint32_t)pgd_kern - PAGE_OFFSET;

    register_interrupt_handler(14, &page_fault);

    set_cr3(pgd_kern_phy_addr);
}

void map(pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t flags)
{
    uint32_t pgd_idx = PGD_INDEX(va);
    uint32_t pte_idx = PTE_INDEX(va);

    pte_t *pte = (pte_t*)(pgd_now[pgd_idx] & PAGE_MASK);
    if (!pte) {
        pte = (pte_t *)alloc_page();
        pgd_now[pgd_idx] = (uint32_t)pte | PAGE_PRESENT | PAGE_WRITE;

        pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
        memset(pte, 0, PAGE_SIZE);
    } else {
        pte = (pte_t*)((uint32_t)pte + PAGE_OFFSET);
    }

    pte[pte_idx] = (pa & PAGE_MASK) | flags;

    asm volatile ("invlpg (%0)" : : "a" (va));
}

void unmap(pgd_t *pgd_now, uint32_t va)
{
    uint32_t pgd_idx = PGD_INDEX(va);
    uint32_t pte_idx = PTE_INDEX(va);

    pte_t *pte = (pte_t*)(pgd_now[pgd_idx] & PAGE_MASK);

    if (!pte) {
        return;
    }

    pte = (pte_t*)((uint32_t)pte + PAGE_OFFSET);

    pte[pte_idx] = 0;

    asm volatile ("invlpg (%0)" : : "a" (va));
}

uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa)
{
    uint32_t pgd_idx = PGD_INDEX(va);
    uint32_t pte_idx = PTE_INDEX(va);

    uint32_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);

    if (!pte) {
          return 0;
    }

    pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);

    if (pte[pte_idx] != 0 && pa) {
         *pa = pte[pte_idx] & PAGE_MASK;
        return 1;
    }

    return 0;
}

void page_fault(register_t *regs)
{
    uint32_t cr2;
    asm volatile ("mov %%cr2, %0" : "=r" (cr2));

    printf("Page fault at 0x%x, virtual faulting address 0x%x\n", regs->eip, cr2);
    printf("Error code: %x\n", regs->err_code);

    // bit 0 为 0 指页面不存在内存里
    if ( !(regs->err_code & 0x1)) {
        printf_color(rc_black, rc_red, "Because the page wasn't present.\n");
    }
    // bit 1 为 0 表示读错误，为 1 为写错误
    if (regs->err_code & 0x2) {
        printf_color(rc_black, rc_red, "Write error.\n");
    } else {
        printf_color(rc_black, rc_red, "Read error.\n");
    }
    // bit 2 为 1 表示在用户模式打断的，为 0 是在内核模式打断的
    if (regs->err_code & 0x4) {
        printf_color(rc_black, rc_red, "In user mode.\n");
    } else {
        printf_color(rc_black, rc_red, "In kernel mode.\n");
    }
    // bit 3 为 1 表示错误是由保留位覆盖造成的
    if (regs->err_code & 0x8) {
        printf_color(rc_black, rc_red, "Reserved bits being overwritten.\n");
    }
    // bit 4 为 1 表示错误发生在取指令的时候
    if (regs->err_code & 0x10) {
        printf_color(rc_black, rc_red, "The fault occurred during an instruction fetch.\n");
    }

    while (1);
}
