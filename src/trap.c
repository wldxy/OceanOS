#include "trap.h"
#include "io.h"
#include "debug.h"
#include "string.h"
#include "process.h"

#define IDT_LENGTH 256

idt_t idts[IDT_LENGTH];

idtr_t idtr;

interrupt_handler_t handlers[256];

uint32_t ticks;

static void setIdt(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

void init_trap() {
    // init Master Slave
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    // IRQ master start from 0x20(32)
    outb(0x21, 0x20);
    // IRQ slave start from 0x28(40)
    outb(0xA1, 0x28);
    // IR2 of master link to slave
    outb(0x21, 0x04);
    // IR2 of slave link to master
    outb(0xA1, 0x02);
    // work as 8086
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    // set interrupt
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    memset((uint8_t *)&handlers, 0, sizeof(interrupt_handler_t) * 256);

    idtr.limit = sizeof(idtr_t) * 256 - 1;
    idtr.base = (uint32_t)&idts;

    memset((uint8_t *)&idts, 0, sizeof(idt_t) * 256);

    setIdt( 0, (uint32_t)isr0,  0x08, 0x8E);
	setIdt( 1, (uint32_t)isr1,  0x08, 0x8E);
	setIdt( 2, (uint32_t)isr2,  0x08, 0x8E);
	setIdt( 3, (uint32_t)isr3,  0x08, 0x8E);
	setIdt( 4, (uint32_t)isr4,  0x08, 0x8E);
	setIdt( 5, (uint32_t)isr5,  0x08, 0x8E);
	setIdt( 6, (uint32_t)isr6,  0x08, 0x8E);
	setIdt( 7, (uint32_t)isr7,  0x08, 0x8E);
	setIdt( 8, (uint32_t)isr8,  0x08, 0x8E);
	setIdt( 9, (uint32_t)isr9,  0x08, 0x8E);
	setIdt(10, (uint32_t)isr10, 0x08, 0x8E);
	setIdt(11, (uint32_t)isr11, 0x08, 0x8E);
	setIdt(12, (uint32_t)isr12, 0x08, 0x8E);
	setIdt(13, (uint32_t)isr13, 0x08, 0x8E);
	setIdt(14, (uint32_t)isr14, 0x08, 0x8E);
	setIdt(15, (uint32_t)isr15, 0x08, 0x8E);
	setIdt(16, (uint32_t)isr16, 0x08, 0x8E);
	setIdt(17, (uint32_t)isr17, 0x08, 0x8E);
	setIdt(18, (uint32_t)isr18, 0x08, 0x8E);
	setIdt(19, (uint32_t)isr19, 0x08, 0x8E);
	setIdt(20, (uint32_t)isr20, 0x08, 0x8E);
	setIdt(21, (uint32_t)isr21, 0x08, 0x8E);
	setIdt(22, (uint32_t)isr22, 0x08, 0x8E);
	setIdt(23, (uint32_t)isr23, 0x08, 0x8E);
	setIdt(24, (uint32_t)isr24, 0x08, 0x8E);
	setIdt(25, (uint32_t)isr25, 0x08, 0x8E);
	setIdt(26, (uint32_t)isr26, 0x08, 0x8E);
	setIdt(27, (uint32_t)isr27, 0x08, 0x8E);
	setIdt(28, (uint32_t)isr28, 0x08, 0x8E);
	setIdt(29, (uint32_t)isr29, 0x08, 0x8E);
	setIdt(30, (uint32_t)isr30, 0x08, 0x8E);
	setIdt(31, (uint32_t)isr31, 0x08, 0x8E);

    setIdt(32, (uint32_t)irq0, 0x08, 0x8E);
    setIdt(33, (uint32_t)irq1, 0x08, 0x8E);
    setIdt(34, (uint32_t)irq2, 0x08, 0x8E);
    setIdt(35, (uint32_t)irq3, 0x08, 0x8E);
    setIdt(36, (uint32_t)irq4, 0x08, 0x8E);
    setIdt(37, (uint32_t)irq5, 0x08, 0x8E);
    setIdt(38, (uint32_t)irq6, 0x08, 0x8E);
    setIdt(39, (uint32_t)irq7, 0x08, 0x8E);
    setIdt(40, (uint32_t)irq8, 0x08, 0x8E);
    setIdt(41, (uint32_t)irq9, 0x08, 0x8E);
    setIdt(42, (uint32_t)irq10, 0x08, 0x8E);
    setIdt(43, (uint32_t)irq11, 0x08, 0x8E);
    setIdt(44, (uint32_t)irq12, 0x08, 0x8E);
    setIdt(45, (uint32_t)irq13, 0x08, 0x8E);
    setIdt(46, (uint32_t)irq14, 0x08, 0x8E);
    setIdt(47, (uint32_t)irq15, 0x08, 0x8E);

	setIdt(255, (uint32_t)isr255, 0x08, 0x8E);

    idt_to_idtr((uint32_t)&idtr);
}

void timer_callback(register_t* reg) {
    ticks++;
    schedule();
}

void init_timer(uint32_t f) {
    register_interrupt_handler(IRQ0, timer_callback);

    ticks = 0;

    uint32_t d = 1193180 / f;

    outb(0x43, 0x36);

    uint8_t low = (uint8_t)(d & 0xFF);
	uint8_t high = (uint8_t)((d >> 8) & 0xFF);

    outb(0x40, low);
    outb(0x40, high);
}

static void setIdt(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idts[num].base_lo = base & 0xFFFF;
	idts[num].base_hi = (base >> 16) & 0xFFFF;

	idts[num].sel = sel;
	idts[num].zero = 0;

	idts[num].flags = flags;  // | 0xidt_s.s60
}

void isr_handler(register_t* reg) {
    if (handlers[reg->int_no]) {
        handlers[reg->int_no](reg);
    } else {
        printf_color(rc_black, rc_blue, "Interrupt ID: %d\n", reg->int_no);
    }
}

void irq_handler(register_t* reg) {
    if (reg->int_no >= 40) {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);

    if (handlers[reg->int_no] != 0) {
        handlers[reg->int_no](reg);
    }
}

void register_interrupt_handler(uint8_t num, interrupt_handler_t h) {
    handlers[num] = h;
}

uint32_t getTicks() {
    return ticks;
}

void sleep(uint32_t t) {
    int st = getTicks();
    while (ticks - st < t) {}
}
