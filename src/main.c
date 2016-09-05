#include "type.h"
#include "monitor.h"
#include "debug.h"
#include "gdt.h"
#include "trap.h"

int main()
{
    init_debug();
    init_gdt();
    init_trap();

    monitor_clear();

    printf_color(rc_black, rc_green, "Hello, os kernel");

    init_timer(200);

    asm volatile ("sti");

	return 0;
}
