#ifndef _EXAMPLES_H_
#define _EXAMPLES_H_
#include <kernel.h>

extern ID tid_lcd;
extern TaskInfo task_lcd;
extern ID dd_i2c0, dd_i2c1;
extern ID fid;

void preemptive_multi_tasking();
void sleep_wake();
void events();
void device();

void task_lcd_func(INT stacd, void* exinf);

#endif
