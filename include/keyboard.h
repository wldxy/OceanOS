#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "trap.h"

void init_keyboard();

void keyboard_handler(register_t* reg);

#endif
