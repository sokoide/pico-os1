#include <kernel.h>

uint32_t global_counter = 0;
time_t global_seconds = 0;

void systimer_handler(void) {
    TaskControlBlock* tcb;
    global_counter++;
    if (global_counter % 100 == 0) {
        global_seconds++;
    }

    for (tcb = wait_queue; tcb != NULL; tcb = tcb->next) {
        if (tcb->wait_time == (RELTIME)TMO_FEVR) {
            continue;
        } else if (tcb->wait_time > (RELTIME)TIMER_PERIOD) {
            tcb->wait_time -= TIMER_PERIOD;
        } else {
            task_queue_remove_entry(&wait_queue, tcb);
            if (tcb->wait_factor == TWFCT_DLY) {
                *tcb->wait_err = E_OK;
            } else {
                *tcb->wait_err = E_TMOUT;
            }
            tcb->state = TS_READY;
            tcb->wait_factor = TWFCT_NON;
            task_queue_add_entry(&ready_queue[tcb->task_pri], tcb);
        }
    }

    // force context switch every TIMER_PERIOD
    if (!dispatch_running && ready_queue[curr_task->task_pri] != NULL &&
        ready_queue[curr_task->task_pri]->next != NULL) {
        task_queue_remove_top(&ready_queue[curr_task->task_pri]);
        task_queue_add_entry(&ready_queue[curr_task->task_pri], curr_task);
    }

    scheduler();
}
