#ifndef _USERMAIN_H_
#define _USERMAIN_H_
#include <kernel.h>

extern ID tid_lcd;
extern TaskInfo task_lcd;
extern ID dd_i2c0, dd_i2c1;
extern ID fid;

// semaphore
extern ID sid;
extern Semaphore sem;

#endif
