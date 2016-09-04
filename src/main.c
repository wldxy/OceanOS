#include "type.h"
#include "console.h"
#include "debug.h"
#include "gdt.h"

int main()
{
    init_debug();
    init_gdt();

    console_clear();

    printf_color(rc_black, rc_green, "Hello, os kernel");

    print_stack_info("test");

	return 0;
}
