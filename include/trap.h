#ifndef _TRAP_H_
#define _TRAP_H_

#include "type.h"

struct idt_struct {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_hi;
}__attribute__((packed));
typedef struct idt_struct idt_t;

struct idtr_struct {
    uint16_t limit;
    uint32_t base;
}__attribute__((packed));
typedef struct idtr_struct idtr_t;

typedef struct register_info {
   uint32_t ds;                                       // Data segment selector
   uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;   // Pushed by pusha.
   uint32_t int_no, err_code;                         // Interrupt number and error code (if applicable)
   uint32_t eip, cs, eflags, useresp, ss;             // Pushed by the processor automatically.
} register_t;

void init_trap();

void init_timer(uint32_t f);

typedef void (*interrupt_handler_t)(register_t *);

void register_interrupt_handler(uint8_t n, interrupt_handler_t h);

void isr_handler(register_t* reg);

void irq_handler(register_t* reg);

extern void idt_to_idtr(uint32_t);

// CPU Error
void isr0();        // 0 - Division by zero exception
void isr1();        // 1 - Debug exception
void isr2();        // 2 - Non maskable interrupt
void isr3();        // 3 - Breakpoint exception
void isr4();        // 4 - Into detected overflow
void isr5();        // 5 - Out of bounds exception
void isr6();        // 6 - Invalid opcode exception
void isr7();        // 7 - No coprocessor exception
void isr8();        // 8 - Double fault (pushes an error code)
void isr9();        // 9 - Coprocessor segment overrun
void isr10();       // 10 - Bad TSS (pushes an error code)
void isr11();       // 11 - Segment not present (pushes an error code)
void isr12();       // 12 - Stack fault (pushes an error code)
void isr13();       // 13 - General protection fault (pushes an error code)
void isr14();       // 14 - Page fault (pushes an error code)
void isr15();       // 15 - Unknown interrupt exception
void isr16();       // 16 - Coprocessor fault
void isr17();       // 17 - Alignment check exception
void isr18();       // 18 - Machine check exception
void isr19();       // 19 - XM SIMD float exception

// Intel
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();

// 32 ~ 255 User
void isr255();

//init IRQ
#define  IRQ0     32    // 电脑系统计时器
#define  IRQ1     33    // 键盘
#define  IRQ2     34    // 与 IRQ9 相接，MPU-401 MD 使用
#define  IRQ3     35    // 串口设备
#define  IRQ4     36    // 串口设备
#define  IRQ5     37    // 建议声卡使用
#define  IRQ6     38    // 软驱传输控制使用
#define  IRQ7     39    // 打印机传输控制使用
#define  IRQ8     40    // 即时时钟
#define  IRQ9     41    // 与 IRQ2 相接，可设定给其他硬件
#define  IRQ10    42    // 建议网卡使用
#define  IRQ11    43    // 建议 AGP 显卡使用
#define  IRQ12    44    // 接 PS/2 鼠标，也可设定给其他硬件
#define  IRQ13    45    // 协处理器使用
#define  IRQ14    46    // IDE0 传输控制使用
#define  IRQ15    47    // IDE1 传输控制使用

void irq0();        // 电脑系统计时器
void irq1();        // 键盘
void irq2();        // 与 IRQ9 相接，MPU-401 MD 使用
void irq3();        // 串口设备
void irq4();        // 串口设备
void irq5();        // 建议声卡使用
void irq6();        // 软驱传输控制使用
void irq7();        // 打印机传输控制使用
void irq8();        // 即时时钟
void irq9();        // 与 IRQ2 相接，可设定给其他硬件
void irq10();       // 建议网卡使用
void irq11();       // 建议 AGP 显卡使用
void irq12();       // 接 PS/2 鼠标，也可设定给其他硬件
void irq13();       // 协处理器使用
void irq14();       // IDE0 传输控制使用
void irq15();       // IDE1 传输控制使用

#endif // _TRAP_H_
