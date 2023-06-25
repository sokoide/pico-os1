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

#define TMO_POL (0)   // 0 timeout
#define TMO_FEVR (-1) // forever

/* enums */
typedef enum {
    TS_NONEXIST = 1,
    TS_READY = 2,
    TS_WAIT = 4,
    TS_DORMANT = 8
} TaskState;

typedef enum {
    TWFCT_NON = 0,
    TWFCT_DLY = 1, // waited by sk_dly_tsk
    TWFCT_SLP = 2, // waited by sk_slp_tsk
} TaskWaitFactor;

/* structs */
typedef struct _task_control_block {
    void* context;

    // task queue pointers
    struct _task_control_block* prev;
    struct _task_control_block* next;

    // task info
    TaskState state;
    FP task_addr;
    PRI task_pri;
    void* stack_addr;
    SZ stack_size;
    INT wakeup_count;

    // wait info
    TaskWaitFactor wait_factor;
    RELTIME wait_time;
    ERR* wait_err;
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
extern TaskControlBlock* wait_queue;
extern UW dispatch_running;

/* global functions */
extern void reset_handler(void);
extern void dispatch_entry(void);
extern void systimer_handler(void);
extern void scheduler(void);
extern void* make_context(UW* sp, UINT ssize, void (*fp)());
extern void task_queue_add_entry(TaskControlBlock** queue,
                                 TaskControlBlock* tcb);
extern void task_queue_remove_top(TaskControlBlock** queue);
extern void task_queue_remove_entry(TaskControlBlock** queue,
                                    TaskControlBlock* tcb);

extern ID sk_create_task(const TaskInfo* ti);
extern ERR sk_start_task(ID id, INT stacd);
extern void sk_exit_task(void);
extern ERR sk_delay_task(RELTIME delay_time);
ERR sk_sleep_task(TIMEOUT timeout);
ERR sk_wakeup_task(ID id);

extern int main(void);
extern int usermain(void);

// inline functions
#define SCB_ICSR 0xE000ED04      // interrupt state register
#define ICSR_PENDSVSET (1 << 28) // PendSV set-pending bit
static inline void dispatch(void) {
    out_w(SCB_ICSR, ICSR_PENDSVSET); // raise PendSV exception
}

#endif
