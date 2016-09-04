#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "type.h"

void console_clear();

void console_putc_color(char c, real_color_t back, real_color_t fore);

void console_write(char *cstr);

void console_write_color(char *cstr, real_color_t back, real_color_t fore);

void console_write_hex(uint32_t n, real_color_t back, real_color_t fore);

void console_write_dec(uint32_t n, real_color_t back, real_color_t fore);

#endif  // INCLUDE_CONSOLE_H_
