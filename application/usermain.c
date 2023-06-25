#include <kernel.h>

// task stack
#define STACK_SIZE 1024
UB stack_task1[STACK_SIZE];
UB stack_task2[STACK_SIZE];

static void delay_ms(UINT ms) {
    UINT cnt = ms / TIMER_PERIOD;

    while (cnt) {
        if ((in_w(SYST_CSR) & SYST_CSR_COUNTFLAG) != 0) {
            cnt--;
        }
    }
}

void task_1(void) {
    tm_putstring("task1 started...\r\n");
    delay_ms(200);
    tm_putstring("task1 exitting...\r\n");
    sk_exit_task();
}

void task_2(void) {
    tm_putstring("task2 started...\r\n");
    delay_ms(200);
    tm_putstring("task2 exitting...\r\n");
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

int usermain(void) {
    tm_putstring("usermain started...\r\n");
    ID t1, t2;
    t1 = sk_create_task(&task1);
    t2 = sk_create_task(&task2);
    sk_start_task(t1, 0);
    sk_start_task(t2, 0);

    tm_putstring("usermain exitting...\r\n");
    return 0;
}
