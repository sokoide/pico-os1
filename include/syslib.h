#ifndef _SYSLIB_H_
#define _SYSLIB_H_

#include "typedef.h"

/*
 * Librafy Functions
 */

/* 32bit register input/output */
// static inline UW in_w(UW adr) { return *(_UW*)adr; }
static inline UW in_w(uintptr_t adr) { return *(_UW*)adr; }

static inline void out_w(uintptr_t adr, UW data) {
    *(_UW*)adr = data;
}

#define OP_CLR 0x3000
static inline void clr_w(uintptr_t adr, UW data) {
    *(_UW*)(adr + OP_CLR) = data;
}

#define OP_SET 0x2000
static inline void set_w(uintptr_t adr, UW data) {
    *(_UW*)(adr + OP_SET) = data;
}

#define OP_XOR 0x1000
static inline void xset_w(uintptr_t adr, UW data) {
    *(_UW*)(adr + OP_XOR) = data;
}

static inline void set_primask(INT pm) {
    __asm__ volatile("msr primask, %0" ::"r"(pm));
}

static inline UW get_primask(void) {
    UW pm;
    __asm__ volatile("mrs %0, primask" : "=r"(pm));
    return pm;
}

static inline UW get_sp(void) {
    UW sp;
    __asm__ volatile("mov %0, sp" : "=r"(sp));
    return sp;
}

/* interrupts */
#define DI(interrupt_status) (interrupt_status = get_primask(), set_primask(1))
#define EI(interrupt_status) (set_primask(interrupt_status))

/* serial */
void tm_com_init(void);

#endif
