#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <_ansi.h>
#include <errno.h>
#include <reent.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <time.h>

int _system(const char*);
int _rename(const char*, const char*);
int isatty(int);
clock_t _times(struct tms*);
int _gettimeofday(struct timeval*, struct timezone*);
void _raise(void);
int _unlink(void);
int _link(void);
int _stat(const char*, struct stat*);
int _fstat(int, struct stat*);
caddr_t _sbrk(int);
int _getpid(int);
int _kill(int, int);
void _exit(int);
int _close(int);
int _open(const char*, int, ...);
int _write(int, char*, int);
int _lseek(int, int, int);
int _read(int, char*, int);
void initialise_monitor_handles(void);

#endif