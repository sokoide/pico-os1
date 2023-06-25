/* vector table */
#include <kerneldef.h>
#include <sysdef.h>
#include <syslib.h>
#include <typedef.h>

/* default hander */
void default_handler(void) {
    while (1)
        ;
}

/* exception vector table */
void (*const vector_tbl[])() __attribute__((section(".vector"))) = {
    (void (*)())(INITIAL_SP), // 0: Top of Stack
    reset_handler,            // 1: Reset
    default_handler,          // 2: NMI
    default_handler,          // 3: Hard Fault
    0,                        // 4: Not Used
    0,                        // 5: Not Used
    0,                        // 6: Not Used
    0,                        // 7: Not Used
    0,                        // 8: Not Used
    0,                        // 9: Not Used
    0,                        // 10: Not Used
    default_handler,          // 11: Svcall
    0,                        // 12: Not Used
    0,                        // 13: Not Used
    dispatch_entry,           // 14: Pend SV
    systimer_handler,         // 15: Systick
    default_handler,          // IRQ 0
    default_handler,          // IRQ 1
    default_handler,          // IRQ 2
    default_handler,          // IRQ 3
    default_handler,          // IRQ 4
    default_handler,          // IRQ 5
    default_handler,          // IRQ 6
    default_handler,          // IRQ 7
    default_handler,          // IRQ 8
    default_handler,          // IRQ 9
    default_handler,          // IRQ 10
    default_handler,          // IRQ 11
    default_handler,          // IRQ 12
    default_handler,          // IRQ 13
    default_handler,          // IRQ 14
    default_handler,          // IRQ 15
    default_handler,          // IRQ 16
    default_handler,          // IRQ 17
    default_handler,          // IRQ 18
    default_handler,          // IRQ 19
    default_handler,          // IRQ 20
    default_handler,          // IRQ 21
    default_handler,          // IRQ 22
    default_handler,          // IRQ 23
    default_handler,          // IRQ 24
    default_handler,          // IRQ 25
    default_handler,          // IRQ 26
    default_handler,          // IRQ 27
    default_handler,          // IRQ 28
    default_handler,          // IRQ 29
    default_handler,          // IRQ 31
};
