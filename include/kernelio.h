#ifndef _KERNELIO_H_
#define _KERNELIO_H_

#include "type.h"

typedef __builtin_va_list va_list;

#define va_start(ap, last)         (__builtin_va_start(ap, last))
#define va_arg(ap, type)           (__builtin_va_arg(ap, type))
#define va_end(ap)

void printf(const char *format, ...);

void printf_color(real_color_t back, real_color_t fore, const char *format, ...);

#endif
