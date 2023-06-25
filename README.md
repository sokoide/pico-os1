# README

## How to build

```sh
cd build
make
```

## How to run

* clone https://github.com/wokwi/rp2040js
* cd build and update rp2040js path in Makefile
* run it by `make run`
* log

``` text
17:53:25.445 [UART0] Unimplemented peripheral write to 24: 67
17:53:25.445 [UART0] Unimplemented peripheral write to 28: 52
initial task started.
usermain started...
usermain exitting...
task1 started...
task1...
task1...
task1...
task1...
...
task1task2 started...
task2...
task2...
task2...
task2...
...
task1...
...
task2...
...
(ctrl-c to quit)
```

## Directories

* build: build directory
* boot: boot files
  * `reset_handler`, `dispatch_entry`, `systimer_handler` are in `vector_table.c`
* kernel: kernel
  * kernel functions
* application: application dir
  * `usermain` called by `main` called by `reset_handler` after initialization

## Task Management

* `task_manager.c` has a ready_queue per priority
  * `TaskControlBlock* ready_queue[MAX_TASK_PRI]`
* It has a `wait_queue` which is not per priority
* When a task is started by `sk_task_start`, the task control block will be added to a tail of an appropriate reqdy_queue
* `systimer_handler` is called every `TIMER_PERIOD` (10ms by default) and does the followings
  * checks tasks in wait_queue and move them into ready_queue if the task's wait_time already passed
  * removes the current task from the top of ready_queue and add it to the tail
  * calls `scheduler`

