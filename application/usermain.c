﻿#include "examples.h"
#include "shell.h"
#include <kernel.h>
#include <stdio.h>

ID sid;
Semaphore sem;

int usermain(void) {
    printf("hello\n");
    printf("usermain started...\n");
    printf("hello\n");
    printf("usermain started...\n");

    printf("TASK_STACK_UPPER_LIMIT:0x%08X\n", (uint32_t)TASK_STACK_UPPER_LIMIT);
    printf("MAIN_TASK_STACK_BASE:  0x%08X\n", (uint32_t)MAIN_TASK_STACK_BASE);
    printf("MAIN_TASK_STACK_END:   0x%08X\n",
           (uint32_t)(MAIN_TASK_STACK_BASE - MAIN_TASK_STACK_SIZE));
    printf("MAIN_TASK SP:          0x%08X\n", get_sp());
    for (int i = 0; i < MAX_TASKS; i++) {
        printf("TASK_STACK_BASE_N[%02d]: 0x%08X\n", i, (uint32_t)(TASK_STACK_BASE_N(i)));
        printf("TASK_STACK_END_N [%02d]: 0x%08X\n", i, (uint32_t)(
               TASK_STACK_BASE_N(i) - TASK_STACK_SIZE));
    }
    fflush(stdout);
    sem.initial_value = 1;
    sem.max_value = 1;
    sid = sk_create_semaphore(&sem);
    printf("Semaphore %d created\n", sid);
    // enable only one of below
    /* preemptive_multi_tasking(); */
    /* sleep_wake(); */
    /* events(); */
    /* device(); */
    run_shell();

    return 0;
}
