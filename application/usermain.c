#include <kernel.h>

// task stack
#define STACK_SIZE 1024
UB stack_task1[STACK_SIZE];
UB stack_task2[STACK_SIZE];
UB stack_task3[STACK_SIZE];
UB stack_task4[STACK_SIZE];

ID t3, t4;

void task_1(void) {
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

void task_2(void) {
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

void task_3(void) {
    tm_putstring("task3 started...\r\n");
    while (1) {
        tm_putstring("task3 sleeping...\r\n");
        sk_delay_task(1000);
        tm_putstring("task4 waking task4...\r\n");
        sk_wakeup_task(t4);
    }
    tm_putstring("task3 exitting...\r\n");
    sk_exit_task();
}

void task_4(void) {
    tm_putstring("task4 started...\r\n");
    while (1) {
        tm_putstring("task4 sleeping...\r\n");
        sk_sleep_task(TMO_FEVR);
        tm_putstring("task4 woke up\r\n");
    }
    tm_putstring("task4 exitting...\r\n");
    sk_exit_task();
}

TaskInfo task1 = {
    .task_attr = TA_HLNG | TA_RNG3 | TA_USERBUF,
    .task = task_1,
    .task_pri = 10,
    .stack_size = sizeof(stack_task1),
    .stack = stack_task1,
};

TaskInfo task2 = {
    .task_attr = TA_HLNG | TA_RNG3 | TA_USERBUF,
    .task = task_2,
    .task_pri = 10,
    .stack_size = sizeof(stack_task2),
    .stack = stack_task2,
};

TaskInfo task3 = {
    .task_attr = TA_HLNG | TA_RNG3 | TA_USERBUF,
    .task = task_3,
    .task_pri = 10,
    .stack_size = sizeof(stack_task3),
    .stack = stack_task3,
};

TaskInfo task4 = {
    .task_attr = TA_HLNG | TA_RNG3 | TA_USERBUF,
    .task = task_4,
    .task_pri = 10,
    .stack_size = sizeof(stack_task4),
    .stack = stack_task4,
};

void preemptive_multi_tasking() {
    ID t1, t2;
    t1 = sk_create_task(&task1);
    t2 = sk_create_task(&task2);
    sk_start_task(t1, 0);
    sk_start_task(t2, 0);
}

void sleep_wake() {
    t3 = sk_create_task(&task3);
    t4 = sk_create_task(&task4);
    sk_start_task(t3, 0);
    sk_start_task(t4, 0);
}

int usermain(void) {
    tm_putstring("usermain started...\r\n");

    // enable only one of below
    preemptive_multi_tasking();
    /* sleep_wake(); */

    tm_putstring("usermain exitting...\r\n");
    return 0;
}
