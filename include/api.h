#ifndef _API_H_
#define _API_H_

#include "kerneldef.h"
#include "sysdef.h"
#include "syslib.h"
#include "typedef.h"

/* constants */
// task attributes
#define TA_HLNG 0x0000001    // the task is written in high level language
#define TA_USERBUF 0x0000020 // stack to use a buffer specified by a user
#define TA_RNG0 0x0000000    // the task to run in ring 0
#define TA_RNG1 0x0000100    // the task to run in ring 1
#define TA_RNG2 0x0000200    // the task to run in ring 2
#define TA_RNG3 0x0000300    // the task to run in ring 3
// task wait attributes
#define TA_TFIFO 0x00000000 // 待ちタスクをFIFO順で管理
#define TA_TPRI 0x00000001  // 待ちタスクを優先度順で管理
#define TA_FIRST 0x00000000 // 待ち行列先頭のタスクを優先
#define TA_CNT 0x00000002   // 要求数の少ないタスクを優先
#define TA_WSGL 0x00000000  // 複数タスクの待ちを許さない
#define TA_WMUL 0x00000008  // 複数タスクの待ちを許す

// timeout
#define TMO_POL (0)   // 0 timeout
#define TMO_FEVR (-1) // forever

// flag
#define TWF_ANDW 0x00000000U   // and wait
#define TWF_ORW 0x00000001U    // or wait
#define TWF_CLR 0x00000010U    // all bit clear
#define TWF_BITCLR 0x00000020U // conditional bit clear

/* structs */
// ref:
// https://github.com/tron-forum/mtkernel_3/blob/master/include/tk/syscall.h
typedef struct {
    void* exinf; // extended info, not used
    ATTR task_attr;
    FP task;
    PRI task_pri;
    SZ stack_size;
    void* stack;
} TaskInfo;

typedef struct {
    ATTR attr;
    UINT initial_value;
} FlagInfo;

/* API  */
// task management
extern void sk_create_taskinfo(TaskInfo* info, ATTR attr, FP task, PRI pri,
                               SZ sz, void* stack);
extern ID sk_create_task(const TaskInfo* ti);
extern ERR sk_start_task(ID id, INT stacd /* not used */);
extern void sk_exit_task(void);
extern ERR sk_delay_task(RELTIME delay_time);
extern ERR sk_sleep_task(TIMEOUT timeout);
extern ERR sk_wakeup_task(ID id);

// flag
extern ID sk_create_flag(const FlagInfo* info);
extern ERR sk_set_flag(ID flgid, UINT set_pattern);
extern ERR sk_clear_flag(ID flgid, UINT clear_pattern);
extern ERR sk_wait_flag(ID flgid, UINT wait_pattern, UINT wait_mode,
                        UINT* p_flag_pattern, TIMEOUT timeout);

#endif
