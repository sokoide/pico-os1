#include <kernel.h>

FlagControlBlock fcb_table[MAX_FLAGS];
SemaphoreControlBlock scb_table[MAX_SEMAPHORE_ID];

// events
ID sk_create_flag(const FlagInfo* info) {
    ID flgid;
    UINT interrupt_status;
    DI(interrupt_status);
    for (flgid = 0; fcb_table[flgid].state != KS_NONEXIST; flgid++)
        ;

    if (flgid < MAX_FLAGS) {
        fcb_table[flgid].state = KS_EXIST;
        fcb_table[flgid].pattern = info->initial_value;
        flgid++;
    } else {
        flgid = E_LIMIT;
    }
    EI(interrupt_status);
    return flgid;
}

static BOOL check_flag(UINT pattern, UINT wait_pattern, UINT wait_mode) {
    if (wait_mode & TWF_ORW) {
        return ((pattern & wait_pattern) != 0);
    } else {
        return ((pattern & wait_pattern) == wait_pattern);
    }
}

ERR sk_set_flag(ID flgid, UINT set_pattern) {
    FlagControlBlock* flgcb;
    TaskControlBlock* tcb;
    ERR err = E_OK;
    UINT interrupt_status;

    if (flgid <= 0 || flgid > MAX_FLAGS)
        return E_ID;

    DI(interrupt_status);
    flgcb = &fcb_table[--flgid];
    if (flgcb->state == KS_EXIST) {
        flgcb->pattern |= set_pattern;

        for (tcb = wait_queue; tcb != NULL; tcb = tcb->next) {
            if (tcb->wait_factor == TWFCT_FLG) {
                if (check_flag(flgcb->pattern, tcb->wait_pattern,
                               tcb->wait_mode)) {
                    task_queue_remove_entry(&wait_queue, tcb);
                    tcb->state = TS_READY;
                    tcb->wait_factor = TWFCT_NON;
                    *tcb->p_flag_pattern = flgcb->pattern;
                    task_queue_add_entry(&ready_queue[tcb->task_pri], tcb);
                    scheduler();

                    if ((tcb->wait_mode & TWF_BITCLR) != 0) {
                        if ((flgcb->pattern &= ~(tcb->wait_pattern)) == 0) {
                            break;
                        }
                    }
                    if ((tcb->wait_mode & TWF_CLR) != 0) {
                        flgcb->pattern = 0;
                        break;
                    }
                }
            }
        }
    } else {
        err = E_NOEXS;
    }

    EI(interrupt_status);
    return err;
}

ERR sk_clear_flag(ID flgid, UINT clear_pattern) {
    FlagControlBlock* flgcb;
    ERR err = E_OK;
    UINT interrupt_status;

    if (flgid <= 0 || flgid > MAX_FLAGS)
        return E_ID;

    DI(interrupt_status);
    flgcb = &fcb_table[--flgid];
    if (flgcb->state == KS_EXIST) {
        flgcb->pattern &= clear_pattern;
    } else {
        err = E_NOEXS;
    }
    EI(interrupt_status);
    return err;
}

ERR sk_wait_flag(ID flgid, UINT wait_pattern, UINT wait_mode,
                 UINT* p_flag_pattern, TIMEOUT timeout) {
    FlagControlBlock* flgcb;
    ERR err = E_OK;
    UINT interrupt_status;

    if (flgid <= 0 || flgid > MAX_FLAGS)
        return E_ID;

    DI(interrupt_status);
    flgcb = &fcb_table[--flgid];
    if (flgcb->state == KS_EXIST) {
        if (check_flag(flgcb->pattern, wait_pattern, wait_mode)) {
            *p_flag_pattern = flgcb->pattern;
            if ((wait_mode & TWF_BITCLR) != 0) {
                flgcb->pattern &= ~wait_pattern;
            }
            if ((wait_mode & TWF_CLR) != 0) {
                flgcb->pattern = 0;
            }
        } else if (timeout == TMO_POL) {
            err = E_TMOUT;
        } else {
            task_queue_remove_top(&ready_queue[curr_task->task_pri]);

            curr_task->state = TS_WAIT;
            curr_task->wait_factor = TWFCT_FLG;
            curr_task->wait_time =
                ((timeout == TMO_FEVR) ? timeout : timeout + TIMER_PERIOD);
            curr_task->wait_pattern = wait_pattern;
            curr_task->wait_mode = wait_mode;
            curr_task->p_flag_pattern = p_flag_pattern;
            curr_task->wait_err = &err;

            task_queue_add_entry(&wait_queue, curr_task);
            scheduler();
        }
    } else {
        err = E_NOEXS;
    }

    EI(interrupt_status);
    return err;
}

// semaphore
ID sk_create_semaphore(const Semaphore* sem) {
    ID semid;
    UINT interrupt_status;

    DI(interrupt_status);
    for (semid = 0; scb_table[semid].state != KS_NONEXIST; semid++)
        ;

    if (semid < MAX_SEMAPHORE_ID) {
        scb_table[semid].state = KS_EXIST;
        scb_table[semid].value = sem->initial_value;
        scb_table[semid].max_value = sem->max_value;
        semid++;
    } else {
        semid = E_LIMIT;
    }
    EI(interrupt_status);
    return semid;
}

ERR sk_signal_semaphore(ID semid, INT cnt) {
    SemaphoreControlBlock* scb;
    TaskControlBlock* tcb;
    ERR err = E_OK;
    UINT interrupt_status;

    if (semid <= 0 || semid > MAX_SEMAPHORE_ID)
        return E_ID;

    DI(interrupt_status);
    scb = &scb_table[--semid];
    if (scb->state == KS_EXIST) {
        scb->value += cnt; // return resource
        if (scb->value <= scb->max_value) {
            for (tcb = wait_queue; tcb != NULL; tcb = tcb->next) {
                if (tcb->wait_factor == TWFCT_SEM) {
                    if (scb->value >= tcb->wait_semaphore) {
                        scb->value -= tcb->wait_semaphore;
                        task_queue_remove_entry(&wait_queue, tcb);

                        tcb->state = TS_READY;
                        tcb->wait_factor = TWFCT_NON;
                        tcb->wait_err = &err;

                        task_queue_add_entry(&ready_queue[tcb->task_pri], tcb);
                        scheduler();
                    } else {
                        break;
                    }
                }
            }
        } else { // resource value > max_value
            scb->value -= cnt;
            err = E_QOVR;
        }
    } else {
        err = E_NOEXS; // unregistered semaphore
    }
    EI(interrupt_status);
    return err;
}

ERR sk_wait_semaphore(ID semid, INT cnt, TIMEOUT timeout) {
    SemaphoreControlBlock* scb;
    ERR err = E_OK;
    UINT interrupt_status;

    if (semid <= 0 || semid > MAX_SEMAPHORE_ID)
        return E_ID;

    DI(interrupt_status);
    scb = &scb_table[--semid];
    if (scb->state == KS_EXIST) {
        if (scb->value >= cnt) {
            scb->value -= cnt;
        } else if (timeout == TMO_POL) {
            // no enough resouce, no wait
            err = E_TMOUT;
        } else {
            // no enough resource, wait
            task_queue_remove_top(&ready_queue[curr_task->task_pri]);

            curr_task->state = TS_WAIT;
            curr_task->wait_factor = TWFCT_SEM;
            // set waitt ime
            curr_task->wait_time =
                ((timeout == TMO_FEVR) ? timeout : timeout + TIMER_PERIOD);
            curr_task->wait_semaphore = cnt;
            curr_task->wait_err = &err;

            task_queue_add_entry(&wait_queue, curr_task);
            scheduler();
        }
    } else {
        err = E_NOEXS; // unregistered semaphore
    }
    EI(interrupt_status);
    return err;
}
