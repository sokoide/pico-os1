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
$ dd if=/dev/zero of=koidos.fat bs=1M count=1
1+0 records in
1+0 records out
1048576 bytes transferred in 0.000558 secs (1879168459 bytes/sec)
$ mkfs.fat -n 'KOIDOS' -F 12 koidos.fat

$ hexdump -C koidos.fat
00000000  eb 3c 90 6d 6b 66 73 2e  66 61 74 00 02 04 01 00  |.<.mkfs.fat.....|
00000010  02 00 02 00 08 f8 02 00  10 00 02 00 00 00 00 00  |................|
00000020  00 00 00 00 80 00 29 19  60 a1 9c 4b 4f 49 44 4f  |......).`..KOIDO|
00000030  53 20 20 20 20 20 46 41  54 31 32 20 20 20 0e 1f  |S     FAT12   ..|
00000040  be 5b 7c ac 22 c0 74 0b  56 b4 0e bb 07 00 cd 10  |.[|.".t.V.......|
00000050  5e eb f0 32 e4 cd 16 cd  19 eb fe 54 68 69 73 20  |^..2.......This |
00000060  69 73 20 6e 6f 74 20 61  20 62 6f 6f 74 61 62 6c  |is not a bootabl|
00000070  65 20 64 69 73 6b 2e 20  20 50 6c 65 61 73 65 20  |e disk.  Please |
00000080  69 6e 73 65 72 74 20 61  20 62 6f 6f 74 61 62 6c  |insert a bootabl|
00000090  65 20 66 6c 6f 70 70 79  20 61 6e 64 0d 0a 70 72  |e floppy and..pr|
000000a0  65 73 73 20 61 6e 79 20  6b 65 79 20 74 6f 20 74  |ess any key to t|
000000b0  72 79 20 61 67 61 69 6e  20 2e 2e 2e 20 0d 0a 00  |ry again ... ...|
000000c0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
000001f0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 55 aa  |..............U.|
00000200  f8 ff ff 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
00000210  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00000600  f8 ff ff 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
00000610  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00000a00  4b 4f 49 44 4f 53 20 20  20 20 20 08 00 00 f3 7b  |KOIDOS     ....{|
00000a10  e2 56 e2 56 00 00 f3 7b  e2 56 00 00 00 00 00 00  |.V.V...{.V......|
00000a20  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00100000

$ brew install mtools
$ echo hello > hello.txt
$ mcopy -i koidos.fat hello.txt ::

$ hexdump -C koidos.fat
...
00000a00  4b 4f 49 44 4f 53 20 20  20 20 20 08 00 00 f3 7b  |KOIDOS     ....{|
00000a10  e2 56 e2 56 00 00 f3 7b  e2 56 00 00 00 00 00 00  |.V.V...{.V......|
00000a20  48 45 4c 4c 4f 20 20 20  54 58 54 20 18 00 39 a1  |HELLO   TXT ..9.|
00000a30  e2 56 e2 56 00 00 39 a1  e2 56 02 00 06 00 00 00  |.V.V..9..V......|
00000a40  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00004a00  68 65 6c 6c 6f 0a 00 00  00 00 00 00 00 00 00 00  |hello...........|
00004a10  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00100000
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
