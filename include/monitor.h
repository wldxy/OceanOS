#ifndef _MONITOR_H_
#define _MONITOR_H_

#include "type.h"

void monitor_clear();

void monitor_putc_color(char c, real_color_t back, real_color_t fore);

void monitor_write(char *cstr);

void monitor_write_color(char *cstr, real_color_t back, real_color_t fore);

void monitor_write_hex(uint32_t n, real_color_t back, real_color_t fore);

void monitor_write_dec(uint32_t n, real_color_t back, real_color_t fore);

#endif  // INCLUDE_MONITOR_H_
