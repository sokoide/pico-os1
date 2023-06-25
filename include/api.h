#ifndef _API_H_
#define _API_H_

#include "kerneldef.h"
#include "sysdef.h"
#include "syslib.h"
#include "typedef.h"

extern ID sk_create_task(const TaskInfo* ti);
extern ERR sk_start_task(ID id, INT stacd);
extern void sk_exit_task(void);
extern ERR sk_delay_task(RELTIME delay_time);
ERR sk_sleep_task(TIMEOUT timeout);
ERR sk_wakeup_task(ID id);

#endif
