#include "examples.h"
#include "shell.h"
#include <kernel.h>
#include <stdio.h>

int usermain(void) {
    printf("hello\r\n");
    printf("usermain started...\r\n");
    printf("hello\r\n");
    printf("usermain started...\r\n");

    printf("TASK_STACK_UPPER_LIMIT: 0x%08X\r\n", TASK_STACK_UPPER_LIMIT);
    printf("MAIN_TASK_STACK_BASE: 0x%08X\r\n", MAIN_TASK_STACK_BASE);
    printf("MAIN_TASK SP: 0x%08X\r\n", get_sp());
    for (int i = 0; i < MAX_TASKS; i++) {
        printf("TASK_STACK_BASE_N[%02d]: 0x%08X\r\n", i, TASK_STACK_BASE_N(i));
    }
    fflush(stdout);

    // enable only one of below
    /* preemptive_multi_tasking(); */
    /* sleep_wake(); */
    /* events(); */
    /* device(); */
    run_shell();

    return 0;
}
