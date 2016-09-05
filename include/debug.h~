#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "console.h"
#include "kernelio.h"
#include "elf.h"

#define assert(x, info)                                     \
	do {                                                	\
		if (!(x)) {                                         \
			print_stack_info(info); 		     		                \
		}                                                   \
	} while (0)

// 编译期间静态检测
#define static_assert(x)                                	\
	switch (x) { case 0: case (x): ; }

void init_debug();

void print_stack_info(const char *msg);

void print_cur_status();

#endif
