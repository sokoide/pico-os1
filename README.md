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
23:20:33.46  [UART0] Unimplemented peripheral write to 24: 67
23:20:33.46  [UART0] Unimplemented peripheral write to 28: 52
main creating tasks...
dispatching to task1...
task1...
task2...
task1...
task2...
task1...
```

## Directories

* build: build directory
* boot: boot files
  * `reset_handler` and `dispatch_entry` are in `vector_table.c`
* kernel: kernel
  * kernel functions
* application: application dir
  * `main` called by `reset_handler` after initialization
