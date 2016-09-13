#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#define sti()   __asm__ ( "sti"::)      // 开中断
#define cli()   __asm__ ( "cli"::)      // 关中断
#define nop()   __asm__ ( "nop"::)      // 空操作
#define iret()  __asm__ ( "iret"::)     // 中断返回

#define set_cr3(_pgd)       \
    __asm__ __volatile__(   \
        "movl %0, %%cr3"    \
        ::"r"(_pgd))

#define _local_irq_save(x) 	\
__asm__ ("pushfl \n\t" 		\
	"popl %0 \n\t" 			\
	"cli" 					\
	:"=g" (x) 				\
	:						\
	:"memory")

#define _local_irq_restore(x)  	\
__asm__ ("pushl %0 \n\t" 		\
	"popfl" 					\
	:							\
	:"g" (x) 					\
	:"memory")

#endif // _SYSTEM_H_
