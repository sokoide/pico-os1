#include "usermain.h"
#include <api.h>
#include <stdio.h>

// task
TaskInfo task1, task2, task3;
ID t1, t2, t3;

// flag
ID fid;
FlagInfo fi;

// semaphore
ID sid;
Semaphore sem;

// I2C
ID dd_i2c0, dd_i2c1;

void task_1(INT stacd, void* exinf) {
    sk_wait_semaphore(sid, 1, TMO_FEVR);
    printf("task1 started...\r\n");
    sk_signal_semaphore(sid, 1);
    /* for (INT i = 0; i < 5000; i++) { */
    /*     printf("task1 looping...\r\n"); */
    /*     sk_delay_task(200); */
    /* } */
    while (1) {
        sk_wait_semaphore(sid, 1, TMO_FEVR);
        printf("task1...\r\n");
        sk_signal_semaphore(sid, 1);
    }
    printf("task1 exitting...\r\n");
    sk_exit_task();
}

void task_2(INT stacd, void* exinf) {
    sk_wait_semaphore(sid, 1, TMO_FEVR);
    printf("task2 started...\r\n");
    sk_signal_semaphore(sid, 1);
    /* for (INT i = 0; i < 10000; i++) { */
    /*     printf("task2 looping...\r\n"); */
    /*     sk_delay_task(100); */
    /* } */
    while (1) {
        sk_wait_semaphore(sid, 1, TMO_FEVR);
        printf("task2...\r\n");
        sk_signal_semaphore(sid, 1);
    }
    printf("task2 exitting...\r\n");
    sk_exit_task();
}

void task_waker(INT stacd, void* exinf) {
    printf("waker started...\r\n");
    while (1) {
        printf("waker sleeping...\r\n");
        sk_delay_task(1000);
        printf("waker waking sleeper...\r\n");
        sk_wakeup_task(t2);
    }
    printf("waker exitting...\r\n");
    sk_exit_task();
}

void task_sleeper(INT stacd, void* exinf) {
    printf("sleeper started...\r\n");
    while (1) {
        printf("sleeper sleeping...\r\n");
        sk_sleep_task(TMO_FEVR);
        printf("sleeper woke up\r\n");
    }
    printf("sleeper exitting...\r\n");
    sk_exit_task();
}

void task_button(INT stacd, void* exinf) {
    printf("task_button started...\r\n");
    while (1) {
        printf("task_button: btn-1 on\r\n");
        sk_set_flag(fid, (1 << 0));
        sk_delay_task(500);
        printf("task_button: btn-2 on\r\n");
        sk_set_flag(fid, (1 << 1));
        sk_delay_task(1000);
    }
    printf("task_button exitting...\r\n");
    sk_exit_task();
}

void task_led1(INT stacd, void* exinf) {
    printf("task_led1 started...\r\n");
    UINT pattern;
    while (1) {
        sk_wait_flag(fid, (1 << 0), TWF_ANDW | TWF_BITCLR, &pattern, TMO_FEVR);
        printf("task_led1: flagged\r\n");
    }
    printf("task_led1 exitting...\r\n");
    sk_exit_task();
}

void task_led2(INT stacd, void* exinf) {
    printf("task_led2 started...\r\n");
    UINT pattern;
    while (1) {
        sk_wait_flag(fid, (1 << 1), TWF_ANDW | TWF_BITCLR, &pattern, TMO_FEVR);
        printf("task_led2: flagged\r\n");
    }
    printf("task_led2 exitting...\r\n");
    sk_exit_task();
}

void preemptive_multi_tasking() {
    sem.attr = TA_TFIFO | TA_FIRST;
    sem.initial_value = 1;
    sem.max_value = 1;
    sid = sk_create_semaphore(&sem);
    sk_create_taskinfo(&task1, TA_HLNG | TA_RNG3, task_1, 10, 0, NULL);
    sk_create_taskinfo(&task2, TA_HLNG | TA_RNG3, task_2, 10, 0, NULL);
    t1 = sk_create_task(&task1);
    t2 = sk_create_task(&task2);
    sk_start_task(t1, 0);
    sk_start_task(t2, 0);
}

void sleep_wake() {
    sk_create_taskinfo(&task1, TA_HLNG | TA_RNG3, task_waker, 10, 0, NULL);
    sk_create_taskinfo(&task2, TA_HLNG | TA_RNG3, task_sleeper, 10, 0, NULL);
    t1 = sk_create_task(&task1);
    t2 = sk_create_task(&task2);
    sk_start_task(t1, 0);
    sk_start_task(t2, 0);
}

void events() {
    fi.attr = TA_TFIFO | TA_WMUL;
    fi.initial_value = 0;
    fid = sk_create_flag(&fi);

    sk_create_taskinfo(&task1, TA_HLNG | TA_RNG3, task_button, 10, 0, NULL);
    sk_create_taskinfo(&task2, TA_HLNG | TA_RNG3, task_led1, 10, 0, NULL);
    sk_create_taskinfo(&task3, TA_HLNG | TA_RNG3, task_led2, 10, 0, NULL);
    t1 = sk_create_task(&task1);
    t2 = sk_create_task(&task2);
    t3 = sk_create_task(&task3);
    sk_start_task(t1, 0);
    sk_start_task(t2, 0);
    sk_start_task(t3, 0);
}

void device() {
    dd_i2c0 = sk_open_device((UB*)"iica", TD_UPDATE);
    if (dd_i2c0 < 0)
        printf("Error I2C0\r\n");
    else
        printf("Open I2C0\r\n");

    dd_i2c1 = sk_open_device((UB*)"iicb", TD_UPDATE);
    if (dd_i2c1 < 0)
        printf("Error I2C1\r\n");
    else
        printf("Open I2C1\r\n");

    tid_lcd = sk_create_task(&task_lcd);
    sk_start_task(tid_lcd, 0);

    sk_sleep_task(TMO_FEVR);
}

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

    // enable only one of below
    /* preemptive_multi_tasking(); */
    sleep_wake();
    /* events(); */
    /* device(); */

    printf("usermain exitting...\r\n");
    return 0;
}
