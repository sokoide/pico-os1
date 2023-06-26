﻿#ifndef _SYSLIB_H_
#define _SYSLIB_H_

#include "typedef.h"

/*
 * Librafy Functions
 */

/* 32bit register input/output */
// static inline UW in_w(UW adr) { return *(_UW*)adr; }
static inline UW in_w(uintptr_t adr) { return *(_UW*)adr; }

__attribute__((unused)) static inline void out_w(uintptr_t adr, UW data) {
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

/* interrupts */
#define DI(intsts) (intsts = get_primask(), set_primask(1))
#define EI(intsts) (set_primask(intsts))

/* serial */
void tm_com_init(void);
UINT tm_putstring(char* str);

#endif