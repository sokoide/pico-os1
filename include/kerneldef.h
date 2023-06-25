#ifndef _KERNELDEF_H_
#define _KERNELDEF_H_

#include "syslib.h"
#include "typedef.h"

/* global variables */
extern void* ctx_table[];
extern UINT cur_task;
extern UINT next_task;

/* global functions */
extern void reset_handler(void);
extern void dispatch_entry(void);
extern void scheduler(void);
extern void* make_context(UW* sp, UINT ssize, void (*fp)());

extern int main(void);

// inline functions
#define SCB_ICSR 0xE000ED04      // interrupt state register
#define ICSR_PENDSVSET (1 << 28) // PendSV set-pending bit
static inline void dispatch(void) {
    out_w(SCB_ICSR, ICSR_PENDSVSET); // raise PendSV exception
}

#endif
