#include <kernel.h>

static void delay_ms(UINT ms) {
    UINT cnt = ms / TIMER_PERIOD;

    while (cnt) {
        if ((in_w(SYST_CSR) & SYST_CSR_COUNTFLAG) != 0) {
            cnt--;
        }
    }
}

int main(void) {
    tm_com_init();
    while (1) {
        tm_putstring("hello\r\n");
        delay_ms(500);
    }
    return 0;
}
