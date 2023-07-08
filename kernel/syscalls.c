#include <kernel.h>
#include <syscalls.h>

extern const void* __bss_end;

uint8_t* heap_end = (uint8_t*)&__bss_end;
uint8_t* prev_heap_end;

static int remap_handle(int fh) {
    // TODO:
    return 0;
}

void initialise_monitor_handles(void) {}

static int get_errno(void) {
    // TODO:
    return (0);
}

static int error(int result) {
    errno = get_errno();
    return result;
}

int _read(int file, char* ptr, int len) {
    // this doesn't work
    // *ptr = sk_uart0_getc();
    // return 1;
    return 0;
}

int _lseek(int file, int ptr, int dir) {
    // TODO:
    return 0;
}

char _write_prev_char = 0;
int _write(int file, char* ptr, int len) {
    for (int i = 0; i < len; i++) {
        if( *ptr == '\n' && _write_prev_char != '\r')
            sk_uart0_putc('\r');
        _write_prev_char = *ptr;
        sk_uart0_putc(*ptr++);
    }
    return len;
}

int _open(const char* path, int flags, ...) {
    // TOOD:
    return 0;
}

int _close(int file) {
    // TODO:
    return 0;
}

void _exit(int n) {
    while (true)
        ;
}

int _kill(int n, int m) {
    // TODO:
    return (0);
}

int _getpid(int n) {
    // TODO:
    return 1;
    n = n;
}

caddr_t _sbrk(int incr) {
    prev_heap_end = heap_end;
    if ((heap_end + incr) > (uint8_t*)TASK_STACK_UPPER_LIMIT)
        return (caddr_t)-1;
    heap_end += incr;
    return (caddr_t)prev_heap_end;
}

int _fstat(int file, struct stat* st) {
    // TODO:
    return 0;
}

int _stat(const char* fname, struct stat* st) {
    // TODO:
    return 0;
}

int _link(void) {
    // TODO:
    return -1;
}

int _unlink(void) {
    // TODO:
    return -1;
}

void _raise(void) {
    // TODO:
    return;
}

int _gettimeofday(struct timeval* tp, struct timezone* tzp) {
    if (tp) {
        tp->tv_sec = 10;
        tp->tv_usec = 0;
    }
    if (tzp) {
        tzp->tz_minuteswest = 0;
        tzp->tz_dsttime = 0;
    }
    return 0;
}

clock_t _times(struct tms* tp) {
    clock_t timeval;

    timeval = 10;
    if (tp) {
        tp->tms_utime = timeval; /* user time */
        tp->tms_stime = 0;       /* system time */
        tp->tms_cutime = 0;      /* user time, children */
        tp->tms_cstime = 0;      /* system time, children */
    }
    return timeval;
};

int _isatty(int fd) {
    // TODO:
    return 1;
    fd = fd;
}

int _system(const char* s) {
    // TODO:
    if (s == NULL)
        return 0;
    errno = ENOSYS;
    return -1;
}

int _rename(const char* oldpath, const char* newpath) {
    // TODO:
    errno = ENOSYS;
    return -1;
}
