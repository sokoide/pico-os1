# README

## Prereq

* Install `arm-none-eabi-gcc`
* Download from `https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads` or use homebrew

## How to build

```sh
cd build
make
```

## How to run

* clone `sokoide` branch from a for at <https://github.com/sokoide/rp2040js/tree/sokoide>

```sh
git clone https://github.com/sokoide/rp2040js.git
git checkout -b sokoide origin/sokoide
```

* cd build and update rp2040js path in Makefile
* run it by `make run`
* log

``` text
initial task started.
hello
usermain started...
hello
usermain started...
TASK_STACK_UPPER_LIMIT: 0x20036F00
MAIN_TASK_STACK_BASE: 0x2003FF00
MAIN_TASK SP: 0x2003F2D0
TASK_STACK_BASE_N[00]: 0x2003EF00
TASK_STACK_BASE_N[01]: 0x2003EB00
TASK_STACK_BASE_N[02]: 0x2003E700
TASK_STACK_BASE_N[03]: 0x2003E300
TASK_STACK_BASE_N[04]: 0x2003DF00
TASK_STACK_BASE_N[05]: 0x2003DB00
TASK_STACK_BASE_N[06]: 0x2003D700
TASK_STACK_BASE_N[07]: 0x2003D300
...
(ctrl-c to quit)
```

* when you make a change in your rpi OS, run `make reset` to reload the new code

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

## Refefrence

* [μT-Kernel 3.0](https://github.com/tron-forum/mtkernel_3)
