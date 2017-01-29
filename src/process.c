#include "process.h"
#include "type.h"
#include "memory.h"
#include "memlib.h"
#include "string.h"

static pid_t cur_pid = 0;
struct process_t *run_head = NULL;
struct process_t *proc_wait_list_head = NULL;
struct process_t *cur_proc = NULL;

void init_proc_schedule() {
    cur_proc = (struct process_t*)(kern_stack_top - STACK_SIZE);
    cur_proc->state = RUNNABLE;
    cur_proc->pid = cur_pid++;
    cur_proc->stack = cur_proc;
    cur_proc->memory = NULL;

    cur_proc->next = cur_proc;
    run_head = cur_proc;
}

void schedule() {
    if (cur_proc != NULL) {
        change_proc(cur_proc->next);
    }
}

pid_t init_thread(int (*fn)(void *), void *arg) {
    struct process_t *new_proc = (struct process_t*)kmalloc(STACK_SIZE);

    memset(new_proc, 0, sizeof(struct process_t));

    new_proc->state = RUNNABLE;
    new_proc->stack = cur_proc;
    new_proc->pid = cur_pid++;
    new_proc->memory = NULL;

    // push stack
    uint32_t *stack_top = (uint32_t*)((uint32_t)new_proc + STACK_SIZE);
    *(--stack_top) = (uint32_t)arg;
    *(--stack_top) = (uint32_t)thread_exit;
    *(--stack_top) = (uint32_t)fn;
    new_proc->context.esp = (uint32_t)new_proc + STACK_SIZE - sizeof(uint32_t) * 3;

    new_proc->context.eflags = 0x200;
    new_proc->next = run_head;

    struct process_t *proc = run_head;
    while (proc->next != run_head) {
        proc = proc->next;
    }
    proc->next = new_proc;

    return new_proc->pid;
}

void change_proc(struct process_t *proc) {
    if (cur_proc != proc) {
        struct process_t *prev = cur_proc;
        cur_proc = proc;
        switch_to(&(prev->context), &(cur_proc->context));
    }
}

void thread_exit() {
    register uint32_t val asm ("eax");

    while (1);
}
