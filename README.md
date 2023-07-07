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
brew install mtools
make fat12


hexdump -C koidos.fat
00000000  eb 3c 90 4d 54 4f 4f 34  30 34 33 00 02 02 01 00  |.<.MTOO4043.....|
00000010  02 70 00 a0 05 f9 03 00  09 00 02 00 00 00 00 00  |.p..............|
00000020  00 00 00 00 00 00 29 0d  15 11 23 4b 4f 49 46 31  |......)...#KOIF1|
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
# user is always 'root'
> whoami
root

# os version
> version
sokoide os ver: 0.0.1

# read memory at 0x10100000 for 32 bytes
> memread 10100000 32
0x10100000: EBCE3C03 54F54FF4
0x10100008: 30433433 02200210
0x10100010: 02007007 0590F93F
0x10100018: 09000020 00000000

> memread 10100e00 32
0x10100E00: 485445C4 4F042002
0x10100E08: 54855845 180100A0
0x10100E10: E26E5625 000000A0
0x10100E18: E26E5625 0C000000

# ls root directory
> ls
V KOIF12
F HELLO.TXT 12
F HELLOJ.TXT 12
F HOGE.TXT 11
F PAGE.TXT 14

# cat files
> cat hoge.txt
I am hoge.

> cat page.txt
You are page.

# show fat info
> fat
bytesPerSector: 512
sectorsPerCluster: 2
reservedSectorCount: 1
tableCount: 2
rootEntryCount: 112
root dir sector count: 7
total_sectors: 1440
tableSize16: 3
* fat start_sector 1
* fat sectors 6
* root_dir start_sector 7
* root_dir sectors 7
* data start_sector 14
- relative jump (eb3c) + nop (90)
- OEM Name
- bytes per sector
- sectors per cluster
- FAT table's 1st sector (reserved sectors)
- FAT table count
- Max entries in root table
- Total sector count
- Media type
- sectors per FAT table
- sectors per track
- head count
eb 3c 90 4d 54 4f 4f 34 30 34 33 00 02 02 01 00 **< **M T O O 4 0 4 3 **********
02 70 00 a0 05 f9 03 00 09 00 02 00 00 00 00 00 **p ****************************
```

## File System

* Contents of `koidos.fat` is converted into `koidos.o` during a link phase in `Makefile` and mapped at 0x10100000 address of the Flash

## Memory Map

* 0x10000000-0x10FFFFFF: 1st half of the flash. OS code is here.
* 0x10100000-0x101FFFFF: 2nd half of the flash. Contents of 720KB `koidos.fat` is here in FAT12 format.
* 0x20000000- (264KB): SRAM


## Refefrence

* [μT-Kernel 3.0](https://github.com/tron-forum/mtkernel_3)
