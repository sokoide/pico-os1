#ifndef _KERNELDEF_H_
#define _KERNELDEF_H_

#include "syslib.h"
#include "typedef.h"

/* constants */
#define MAX_TASK_ID 32
#define MAX_TASK_PRI 16

#define TA_HLNG 0x0000001    // the task is written in high level language
#define TA_USERBUF 0x0000020 // stack to use a buffer specified by a user
#define TA_RNG0 0x0000000    // the task to run in ring 0
#define TA_RNG1 0x0000100    // the task to run in ring 1
#define TA_RNG2 0x0000200    // the task to run in ring 2
#define TA_RNG3 0x0000300    // the task to run in ring 3

/* structs */
typedef enum {
    TS_NONEXIST = 1,
    TS_READY = 2,
    TS_WAIT = 4,
    TS_DORMANT = 8
} TaskState;

typedef struct _task_control_block {
    void* context;
    struct _task_control_block* prev;
    struct _task_control_block* next;
    TaskState state;
    FP task_addr;
    PRI task_pri;
    void* stack_addr;
    SZ stack_size;
} TaskControlBlock;

typedef struct {
    ATTR task_attr;
    FP task;
    PRI task_pri;
    SZ stack_size;
    void* stack;
} TaskInfo;

/* global variables */
extern TaskControlBlock tcb_table[];
extern TaskControlBlock* ready_queue[];
extern TaskControlBlock* curr_task;
extern TaskControlBlock*
    scheduled_task; // scheduled task which will be executed after the curr_task

/* global functions */
extern void reset_handler(void);
extern void dispatch_entry(void);
extern void scheduler(void);
extern void* make_context(UW* sp, UINT ssize, void (*fp)());
extern void task_queue_add_entry(TaskControlBlock** queue,
                                 TaskControlBlock* tcb);
extern void task_queue_remove_top(TaskControlBlock** queue);
extern void task_queue_remove_entry(TaskControlBlock** queue,
                                    TaskControlBlock* tcb);

ID sk_create_task(const TaskInfo* ti);
ERR sk_start_task(ID id, INT stacd);
void sk_exit_task(void);

extern int main(void);
extern int usermain(void);

// inline functions
#define SCB_ICSR 0xE000ED04      // interrupt state register
#define ICSR_PENDSVSET (1 << 28) // PendSV set-pending bit
static inline void dispatch(void) {
    out_w(SCB_ICSR, ICSR_PENDSVSET); // raise PendSV exception
}

#endif
