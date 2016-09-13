#include "memory.h"
#include "type.h"
#include "monitor.h"
#include "debug.h"
#include "gdt.h"
#include "trap.h"
#include "memory.h"
#include "multiboot.h"
#include "keyboard.h"
#include "memlib.h"
#include "process.h"
#include "test.h"
#include "system.h"

void _init();

multiboot_t *glb_mboot_ptr;

char kern_stack[STACK_SIZE];
uint32_t kern_stack_top;

__attribute__((section(".init.data"))) pgd_t *pgd_tmp  = (pgd_t *)0x1000;
__attribute__((section(".init.data"))) pgd_t *pte_low  = (pgd_t *)0x2000;
__attribute__((section(".init.data"))) pgd_t *pte_high = (pgd_t *)0x3000;

__attribute__((section(".init.text"))) void init_kernel()
{
    pgd_tmp[0] = (uint32_t)pte_low | PAGE_PRESENT | PAGE_WRITE;
    pgd_tmp[PGD_INDEX(PAGE_OFFSET)] = (uint32_t)pte_high | PAGE_PRESENT | PAGE_WRITE;

    int i;
    for (i = 0; i < 1024; i++) {
        pte_low[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
    }

        for (i = 0; i < 1024; i++) {
        pte_high[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
    }

    asm volatile ("mov %0, %%cr3" : : "r" (pgd_tmp));

    uint32_t cr0;
    asm volatile ("mov %%cr0, %0" : "=r" (cr0));
    cr0 |= 0x80000000;
    asm volatile ("mov %0, %%cr0" : : "r" (cr0));

    kern_stack_top = ((uint32_t)kern_stack + STACK_SIZE) & 0xFFFFFFF0;
    asm volatile ("mov %0, %%esp\n\t"
            "xor %%ebp, %%ebp" : : "r" (kern_stack_top));

    glb_mboot_ptr = mboot_ptr_tmp + PAGE_OFFSET;

   _init();
}

void _init() {
    init_debug();
    init_gdt();
    init_trap();
    init_page();
    init_vmm();

    monitor_clear();
    init_timer(200);
    init_proc_schedule();
    init_keyboard();
    sti();

    showStart();

    //init_thread(testb, NULL, 2);
    //init_thread(testa, NULL, 2);
    //asm volatile ("sti");

    //uint32_t p = alloc_page();
    //printf_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n", p);

    //void *addr1 = kmalloc(50);
    //printf("kmalloc    50 byte in 0x%X\n", addr1);

    while (1) {
        asm volatile ("hlt");
    }
}
