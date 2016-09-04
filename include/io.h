#ifndef _IO_H_
#define _IO_H_

#include "type.h"

static inline void io_delay(){
    asm volatile (
        "jmp 1f; 1: jmp 1f; 1:"
        );
}

/* inb & outb */
static inline uint8_t inb(uint16_t port){
    uint8_t ret;
    asm volatile( "inb %1, %0" : "=a" (ret) : "dN" (port));
    io_delay();
    return ret;
}

static inline void outb(uint16_t port, uint8_t data){
    asm volatile( "outb %1, %0" :: "dN" (port), "a" (data));
    io_delay();
}

/* inw & outw */
static inline uint16_t inw(uint16_t port){
    uint16_t ret;
    asm volatile( "inw %1, %0" : "=a" (ret) : "dN" (port));
    io_delay();
    return ret;
}

static inline void outw(uint16_t port, uint16_t data){
    asm volatile( "outw %1, %0" :: "dN" (port), "a" (data));
    io_delay();
}

/* insb & outsb */
static inline void insb(uint32_t port, void *addr, int32_t cnt) {
	asm volatile(
            "cld;" "repne;" "insb"
			 :"=D" (addr), "=c" (cnt)
			 :"d" (port), "0" (addr), "1" (cnt)
			 :"memory", "cc");
    io_delay();
}

static inline void outsb(uint32_t port, void *addr, int32_t cnt) {
	asm volatile(
             "cld;" "repne;" "outsb"
			 :"=S" (addr), "=c" (cnt)
			 :"d" (port), "0" (addr), "1" (cnt)
			 :"cc");
    io_delay();
}

/* insl & outsl */
static inline void insl(uint32_t port, void *addr, int32_t cnt) {
	asm volatile(
             "cld;" "repne;" "insl"
			 :"=D" (addr), "=c" (cnt)
			 :"d" (port), "0" (addr), "1" (cnt)
			 :"memory", "cc");
    io_delay();
}

static inline void outsl(uint32_t port, void *addr, int32_t cnt) {
	asm volatile(
             "cld;" "repne;" "outsl"
			 :"=S" (addr), "=c" (cnt)
			 :"d" (port), "0" (addr), "1" (cnt)
			 :"cc");
    io_delay();
}

#endif // _IO_H_
