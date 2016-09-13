#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "type.h"
#include "memory.h"

typedef
enum process_state {
    UNINIT = 0,
    SLEEPING = 1,
    RUNNABLE = 2,
    ZOMBIE = 3,
} process_state;

struct context_info {
    uint32_t esp;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t esi;
    uint32_t edi;
    uint32_t eflags;
};

struct memory_map {
    pgd_t* pgd;
};

struct process_t {
    volatile process_state state;
    pid_t pid;
    void* stack;
    struct memory_map* memory;
    struct context_info context;
    struct process_t* next;
};

extern struct process_t *run_head;

extern struct process_t *proc_wait_list_head;

extern struct process_t *cur_proc;

void init_proc_schedule();

pid_t init_thread(int (*fn)(void *), void *arg);

void thread_exit();

void schedule();

void change_proc(struct process_t *proc);

void switch_to(struct context_info* prev, struct context_info* next);

#endif // _PROCESS_H_
