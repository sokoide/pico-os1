#include <kernel.h>

TaskControlBlock tcb_table[MAX_TASK_ID];
TaskControlBlock* ready_queue[MAX_TASK_PRI];
TaskControlBlock* curr_task;
TaskControlBlock* scheduled_task;
TaskControlBlock* wait_queue;
UW dispatch_running = 0;

ID initial_task_id;
UB stack_initialtask[256];
void initial_task_func(INT stacd, void* exinf);

TaskInfo initial_task = {
    .task_attr = TA_HLNG | TA_RNG0 | TA_USERBUF,
    .task = initial_task_func,
    .task_pri = 1,
    .stack_size = sizeof(stack_initialtask),
    .stack = stack_initialtask,
};

// functions
int main(void) {
    // initialize tcb_table
    for (INT i = 0; i < MAX_TASK_ID; i++) {
        tcb_table[i].state = TS_NONEXIST;
    }

    initial_task_id = sk_create_task(&initial_task);
    sk_start_task(initial_task_id, 0);

    while (1)
        ; // never reaches here
}

void task_queue_add_entry(TaskControlBlock** queue, TaskControlBlock* tcb) {
    TaskControlBlock* queue_end;

    if (*queue == NULL) {
        // empty queue, add @ head
        *queue = tcb;
        tcb->prev = tcb;
    } else {
        // add @ tail
        queue_end = (*queue)->prev;
        queue_end->next = tcb;
        tcb->prev = queue_end;
        (*queue)->prev = tcb;
    }
    tcb->next = NULL;
}

void task_queue_remove_top(TaskControlBlock** queue) {
    TaskControlBlock* top;

    if (*queue == NULL)
        return;

    top = *queue;
    *queue = top->next;
    if (*queue != NULL) {
        (*queue)->prev = top->prev;
    }
}

void task_queue_remove_entry(TaskControlBlock** queue, TaskControlBlock* tcb) {
    if (*queue == tcb) {
        // the entry is @ queue head
        task_queue_remove_top(queue);
    } else {
        (tcb->prev)->next = tcb->next;
        if (tcb->next != NULL) {
            (tcb->next)->prev = tcb->prev;
        } else {
            (*queue)->prev = tcb->prev;
        }
    }
}

void scheduler(void) {
    INT i;

    for (i = 0; i < MAX_TASK_PRI; i++) {
        if (ready_queue[i] != NULL)
            break;
    }

    if (i < MAX_TASK_PRI) {
        scheduled_task = ready_queue[i];
    } else {
        scheduled_task = NULL;
    }
    if (scheduled_task != curr_task && !dispatch_running) {
        dispatch();
    }
}

void initial_task_func(INT stacd, void* exinf) {
    tm_com_init();
    tm_putstring("initial task started.\r\n");

    usermain();
    sk_exit_task();
}

ID sk_create_task(const TaskInfo* ti) {
    UINT interrupt_status;
    ID id;
    INT i;

    if ((ti->task_attr & ~TA_RNG3) != (TA_HLNG | TA_USERBUF))
        return E_RSATR;
    if (ti->task_pri <= 0 || ti->task_pri > MAX_TASK_PRI)
        return E_PAR;
    if (ti->stack_size == 0)
        return E_PAR;

    DI(interrupt_status);

    for (i = 0; i < MAX_TASK_ID; i++) {
        // find unused TCB
        if (tcb_table[i].state == TS_NONEXIST)
            break;
    }

    // init tcb_table
    if (i < MAX_TASK_ID) {
        tcb_table[i].state = TS_DORMANT;
        tcb_table[i].prev = NULL;
        tcb_table[i].next = NULL;

        tcb_table[i].task_addr = ti->task;
        tcb_table[i].task_pri = ti->task_pri;
        tcb_table[i].stack_size = ti->stack_size;
        tcb_table[i].stack_addr = ti->stack;

        id = i + 1;
    } else {
        id = (ID)E_LIMIT;
    }

    EI(interrupt_status);
    return id;
}

ERR sk_start_task(ID id, INT stacd) {
    TaskControlBlock* tcb;
    UINT interrupt_status;
    ERR err = E_OK;

    if (id <= 0 || id > MAX_TASK_ID)
        return E_ID;
    DI(interrupt_status);

    tcb = &tcb_table[id - 1];
    if (tcb->state == TS_DORMANT) {
        tcb->state = TS_READY;
        tcb->context =
            make_context(tcb->stack_addr, tcb->stack_size, tcb->task_addr);
        task_queue_add_entry(&ready_queue[tcb->task_pri], tcb);
        scheduler();
    } else {
        err = E_OBJ;
    }

    EI(interrupt_status);
    return err;
}

void sk_exit_task(void) {
    UINT interrupt_status;

    DI(interrupt_status);

    curr_task->state = TS_DORMANT;
    task_queue_remove_top(&ready_queue[curr_task->task_pri]);

    scheduler();
    EI(interrupt_status);
}

ERR sk_delay_task(RELTIME delay_time) {
    UINT interrupt_status;
    ERR err = E_OK;

    DI(interrupt_status);
    if (delay_time > 0) {
        task_queue_remove_top(&ready_queue[curr_task->task_pri]);

        curr_task->state = TS_WAIT;
        curr_task->wait_factor = TWFCT_DLY;
        curr_task->wait_time = delay_time + TIMER_PERIOD;
        curr_task->wait_err = &err;

        task_queue_add_entry(&wait_queue, curr_task);
        scheduler();
    }
    EI(interrupt_status);
    return err;
}

ERR sk_sleep_task(TIMEOUT timeout) {
    UINT interrupt_status;
    ERR err = E_OK;

    DI(interrupt_status);
    if (curr_task->wakeup_count > 0) {
        curr_task->wakeup_count--;
    } else {
        task_queue_remove_top(&ready_queue[curr_task->task_pri]);

        curr_task->state = TS_WAIT;
        curr_task->wait_factor = TWFCT_SLP;
        curr_task->wait_time =
            (timeout == TMO_FEVR) ? timeout : (timeout + TIMER_PERIOD);
        curr_task->wait_err = &err;

        task_queue_add_entry(&wait_queue, curr_task);
        scheduler();
    }
    EI(interrupt_status);
    return err;
}

ERR sk_wakeup_task(ID id) {
    TaskControlBlock* tcb;
    UINT interrupt_status;
    ERR err = E_OK;

    if (id <= 0 || id > MAX_TASK_ID)
        return E_ID;

    DI(interrupt_status);
    tcb = &tcb_table[id - 1];
    if ((tcb->state == TS_WAIT) && (tcb->wait_factor == TWFCT_SLP)) {

        task_queue_remove_entry(&wait_queue, tcb);

        tcb->state = TS_READY;
        tcb->wait_factor = TWFCT_NON;

        task_queue_add_entry(&ready_queue[tcb->task_pri], tcb);
        scheduler();
    } else if (tcb->state == TS_READY || tcb->state == TS_WAIT) {
        tcb->wakeup_count++;
    } else {
        err = E_OBJ;
    }

    EI(interrupt_status);
    return err;
}
