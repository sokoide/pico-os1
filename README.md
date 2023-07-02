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
cd rp2040js
git checkout -b sokoide origin/sokoide
npm install
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

## How to make a fat12 FS

```sh
$ brew install mtools
$ mformat -f 720 -C -i koidos.fat ::
$ $ hexdump -C koidos.fat
00000000  eb 3c 90 4d 54 4f 4f 34  30 34 33 00 02 02 01 00  |.<.MTOO4043.....|
00000010  02 70 00 a0 05 f9 03 00  09 00 02 00 00 00 00 00  |.p..............|
00000020  00 00 00 00 00 00 29 6b  61 60 7d 4e 4f 20 4e 41  |......)ka`}NO NA|
00000030  4d 45 20 20 20 20 46 41  54 31 32 20 20 20 fa 31  |ME    FAT12   .1|
00000040  c0 8e d8 8e c0 fc b9 00  01 be 00 7c bf 00 80 f3  |...........|....|
00000050  a5 ea 56 00 00 08 b8 01  02 bb 00 7c ba 80 00 b9  |..V........|....|
00000060  01 00 cd 13 72 05 ea 00  7c 00 00 cd 19 00 00 00  |....r...|.......|
00000070  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
000001b0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 80 00  |................|
000001c0  01 00 01 01 09 4f 00 00  00 00 a0 05 00 00 00 00  |.....O..........|
000001d0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
...

$ echo "hello world" > hello.txt
$ echo "konnnichiwa" > helloj.txt
$ mcopy -i koidos.fat hello.txt ::
$ mcopy -i koidos.fat helloj.txt ::

$ hexdump -C koidos.fat
...
00000e00  48 45 4c 4c 4f 20 20 20  54 58 54 20 18 00 6a b7  |HELLO   TXT ..j.|
00000e10  e2 56 e2 56 00 00 6a b7  e2 56 02 00 0c 00 00 00  |.V.V..j..V......|
00000e20  48 45 4c 4c 4f 4a 20 20  54 58 54 20 18 00 35 b8  |HELLOJ  TXT ..5.|
00000e30  e2 56 e2 56 00 00 35 b8  e2 56 03 00 0c 00 00 00  |.V.V..5..V......|
00000e40  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00001c00  68 65 6c 6c 6f 20 77 6f  72 6c 64 0a 00 00 00 00  |hello world.....|
00001c10  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00002000  6b 6f 6e 6e 6e 69 63 68  69 77 61 0a 00 00 00 00  |konnnichiwa.....|
00002010  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
...
```

## Project Directories

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

## Shell

* Very limited commands are supported
* Take a look at [shell.c](application/shell.c)
* Example commands to dump the filesystem at 0x1010000 for 32 bytes
* Notice that that's the contents of `koidos.fat` above

```txt
> whoami
root
> version
sokoide os ver: 0.0.1
> memread 10100000 32
0x10100000: EBCE3C03 6B666636
0x10100008: 66166146 02400410
0x10100010: 02000020 0880F82F
0x10100018: 10010020 00000000
> memread 10100a00 32
0x10100A00: 4BF44F94 4F345305
0x10100A08: 20022002 00000030
0x10100A10: E26E5625 00000030
0x10100A18: E26E5605 00000000
```

## File System

* Contents of `koidos.fat` is converted into `koidos.o` during a link phase in `Makefile` and mapped at 0x18000000 address of the Flash

## Memory Map

* 0x10000000-0x10FFFFFF: 1st half of the flash. OS code is here.
* 0x10100000-0x101FFFFF: 2nd half of the flash. Contents of `koidos.fat` is here in FAT12 format.
* 0x20000000- (264KB): SRAM


## Refefrence

* [μT-Kernel 3.0](https://github.com/tron-forum/mtkernel_3)
