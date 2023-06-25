#ifndef _KERNELDEF_H_
#define _KERNELDEF_H_

#include "syslib.h"
#include "typedef.h"

/* constants */
#define MAX_TASK_ID 32
#define MAX_TASK_PRI 16
#define MAX_FLAG_ID 8

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
    TWFCT_FLG = 3, // waited by sk_wait_flag
} TaskWaitFactor;

typedef enum { KS_NONEXIST = 0, KS_EXIST = 1 } KernelState;

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

    // event flag wait info
    UINT wait_pattern;
    UINT wait_mode;
    UINT* p_flag_pattern; // flag pattern when wait canceled
} TaskControlBlock;

typedef struct {
    KernelState state;
    UINT pattern;
} FlagControlBlock;

/* global variables */
extern TaskControlBlock tcb_table[];
extern TaskControlBlock* ready_queue[];
extern TaskControlBlock* curr_task;
extern TaskControlBlock*
    scheduled_task; // scheduled task which will be executed after the curr_task
extern TaskControlBlock* wait_queue;
extern UW dispatch_running;
extern FlagControlBlock fcb_table[];

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

extern int main(void);
extern int usermain(void);

// inline functions
#define SCB_ICSR 0xE000ED04      // interrupt state register
#define ICSR_PENDSVSET (1 << 28) // PendSV set-pending bit
static inline void dispatch(void) {
    out_w(SCB_ICSR, ICSR_PENDSVSET); // raise PendSV exception
}

#endif
