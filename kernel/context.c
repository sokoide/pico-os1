#include <knldef.h>
#include <sysdef.h>
#include <syslib.h>
#include <typedef.h>

// execution context
typedef struct {
    UW r_[8]; /* R4-R11 register backuped by the exception handler */
    UW r[4];  /* R0-R3 register backup */
    UW ip;    /* R12 register backup */
    UW lr;    /* lr register backup */
    UW pc;    /* pc register backup */
    UW xpsr;  /* xpsr register bacup */
} StackFrame;

void* make_context(UW* sp, UINT ssize, void (*fp)()) {
    StackFrame* sfp;

    // sfp -> execution context pointer
    sfp = (StackFrame*)((UW)sp + ssize);
    sfp--;

    // init execution context pointer
    sfp->xpsr = 0x01000000;
    sfp->pc = (UW)fp & ~0x00000001UL;

    return (void*)sfp;
}
