#include <api.h>

// task
#define STACK_SIZE 1024
UB stack_task1[STACK_SIZE];
UB stack_task2[STACK_SIZE];
UB stack_task3[STACK_SIZE];
TaskInfo task1, task2, task3;
ID t1, t2, t3;

// flag
ID fid;
FlagInfo fi;

void task_1(INT stacd, void* exinf) {
    tm_putstring("task1 started...\r\n");
    /* for (INT i = 0; i < 5000; i++) { */
    /*     tm_putstring("task1 looping...\r\n"); */
    /*     sk_delay_task(200); */
    /* } */
    while (1) {
        tm_putstring("task1...\r\n");
    }
    tm_putstring("task1 exitting...\r\n");
    sk_exit_task();
}

void task_2(INT stacd, void* exinf) {
    tm_putstring("task2 started...\r\n");
    /* for (INT i = 0; i < 10000; i++) { */
    /*     tm_putstring("task2 looping...\r\n"); */
    /*     sk_delay_task(100); */
    /* } */
    while (1) {
        tm_putstring("task2...\r\n");
    }
    tm_putstring("task2 exitting...\r\n");
    sk_exit_task();
}

void task_waker(INT stacd, void* exinf) {
    tm_putstring("waker started...\r\n");
    while (1) {
        tm_putstring("waker sleeping...\r\n");
        sk_delay_task(1000);
        tm_putstring("waker waking sleeper...\r\n");
        sk_wakeup_task(t2);
    }
    tm_putstring("waker exitting...\r\n");
    sk_exit_task();
}

void task_sleeper(INT stacd, void* exinf) {
    tm_putstring("sleeper started...\r\n");
    while (1) {
        tm_putstring("sleeper sleeping...\r\n");
        sk_sleep_task(TMO_FEVR);
        tm_putstring("sleeper woke up\r\n");
    }
    tm_putstring("sleeper exitting...\r\n");
    sk_exit_task();
}

void task_button(INT stacd, void* exinf) {
    tm_putstring("task_button started...\r\n");
    while (1) {
        tm_putstring("task_button: btn-1 on\r\n");
        sk_set_flag(fid, (1 << 0));
        sk_delay_task(500);
        tm_putstring("task_button: btn-2 on\r\n");
        sk_set_flag(fid, (1 << 1));
        sk_delay_task(1000);
    }
    tm_putstring("task_button exitting...\r\n");
    sk_exit_task();
}

void task_led1(INT stacd, void* exinf) {
    tm_putstring("task_led1 started...\r\n");
    UINT pattern;
    while (1) {
        sk_wait_flag(fid, (1 << 0), TWF_ANDW | TWF_BITCLR, &pattern, TMO_FEVR);
        tm_putstring("task_led1: flagged\r\n");
    }
    tm_putstring("task_led1 exitting...\r\n");
    sk_exit_task();
}

void task_led2(INT stacd, void* exinf) {
    tm_putstring("task_led2 started...\r\n");
    UINT pattern;
    while (1) {
        sk_wait_flag(fid, (1 << 1), TWF_ANDW | TWF_BITCLR, &pattern, TMO_FEVR);
        tm_putstring("task_led2: flagged\r\n");
    }
    tm_putstring("task_led2 exitting...\r\n");
    sk_exit_task();
}

void preemptive_multi_tasking() {
    sk_create_taskinfo(&task1, TA_HLNG | TA_RNG3 | TA_USERBUF, task_1, 10,
                       sizeof(stack_task1), &stack_task1);
    sk_create_taskinfo(&task2, TA_HLNG | TA_RNG3 | TA_USERBUF, task_2, 10,
                       sizeof(stack_task2), &stack_task2);
    t1 = sk_create_task(&task1);
    t2 = sk_create_task(&task2);
    sk_start_task(t1, 0);
    sk_start_task(t2, 0);
}

void sleep_wake() {
    sk_create_taskinfo(&task1, TA_HLNG | TA_RNG3 | TA_USERBUF, task_waker, 10,
                       sizeof(stack_task1), &stack_task1);
    sk_create_taskinfo(&task2, TA_HLNG | TA_RNG3 | TA_USERBUF, task_sleeper, 10,
                       sizeof(stack_task2), &stack_task2);
    t1 = sk_create_task(&task1);
    t2 = sk_create_task(&task2);
    sk_start_task(t1, 0);
    sk_start_task(t2, 0);
}

void events() {
    fi.attr = TA_TFIFO | TA_WMUL;
    fi.initial_value = 0;
    fid = sk_create_flag(&fi);

    sk_create_taskinfo(&task1, TA_HLNG | TA_RNG3 | TA_USERBUF, task_button, 10,
                       sizeof(stack_task1), &stack_task1);
    sk_create_taskinfo(&task2, TA_HLNG | TA_RNG3 | TA_USERBUF, task_led1, 10,
                       sizeof(stack_task2), &stack_task2);
    sk_create_taskinfo(&task3, TA_HLNG | TA_RNG3 | TA_USERBUF, task_led2, 10,
                       sizeof(stack_task3), &stack_task3);
    t1 = sk_create_task(&task1);
    t2 = sk_create_task(&task2);
    t3 = sk_create_task(&task3);
    sk_start_task(t1, 0);
    sk_start_task(t2, 0);
    sk_start_task(t3, 0);
}

int usermain(void) {
    tm_putstring("usermain started...\r\n");

    // enable only one of below
    /* preemptive_multi_tasking(); */
    sleep_wake();
    /* events(); */

    tm_putstring("usermain exitting...\r\n");
    return 0;
}
