# README

## How to build

```sh
cd build
make
```

## How to run

``sh
clone https://github.com/wokwi/rp2040js
update rp2040js path in Makefile
make run
```

## Directories

* build: build directory
* boot: boot files
  * `reset_handler` is the entry
* kernel: kernel
  * kernel functions
* application: application dir
  * `main` called by `reset_handler` after initialization
