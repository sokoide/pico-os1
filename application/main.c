#include <kernel.h>

// task management
UINT cur_task = 0;
UINT next_task = 0;

#define MAX_CONTEXTS 2
void* ctx_table[MAX_CONTEXTS];

// task stack
#define STACK_SIZE 1024
UW stack_1[STACK_SIZE];
UW stack_2[STACK_SIZE];

static void delay_ms(UINT ms) {
    UINT cnt = ms / TIMER_PERIOD;

    while (cnt) {
        if ((in_w(SYST_CSR) & SYST_CSR_COUNTFLAG) != 0) {
            cnt--;
        }
    }
}

void task_1(void) {
    while (1) {
        tm_putstring("task1...\r\n");
        delay_ms(500);

        next_task = 2;
        dispatch();
    }
}

void task_2(void) {
    while (1) {
        tm_putstring("task2...\r\n");
        delay_ms(500);

        next_task = 1;
        dispatch();
    }
}

int main(void) {
    tm_com_init();
    tm_putstring("main creating tasks...\r\n");
    ctx_table[0] = make_context(stack_1, sizeof(stack_1), task_1);
    ctx_table[1] = make_context(stack_2, sizeof(stack_2), task_2);

    tm_putstring("dispatching to task1...\r\n");
    next_task = 1;
    dispatch();

    return 0;
}
