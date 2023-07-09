#include <kernel.h>

void uart_init(void) {
    // https://github.com/raspberrypi/pico-sdk/blob/master/src/rp2_common/hardware_uart/uart.c#L128C1-L155C1
    // set baudrate
    out_w(UART0_BASE + UARTx_IBRD, 67);
    out_w(UART0_BASE + UARTx_FBRD, 52);
    // set format
    out_w(UART0_BASE + UARTx_LCR_H, 0x70);
    // enable UART
    out_w(UART0_BASE + UARTx_CR, UART_CR_RXE | UART_CR_TXE | UART_CR_EN);
}

char sk_uart0_getc() {
    while ((in_w(UART0_BASE + UARTx_FR) & UART_FR_RXFF) != 0)
        sk_delay_task(1); /* wait for incoming FIFO queue */

    char c = (in_w(UART0_BASE + UARTx_DR) & 0xFF);
    // echo back
    sk_uart0_putc(c);
    return c;
}

int sk_uart0_putc(char c) {
    while ((in_w(UART0_BASE + UARTx_FR) & UART_FR_TXFF) != 0)
        ; /* wait for outgoing FIFO queue */
    out_w(UART0_BASE + UARTx_DR, c);
    return 1;
}

int sk_uart0_gets_s(char* s, int len) {
    int ret = 0;
    char c;
    for (int i = 0; i < len - 1; i++) {
        c = sk_uart0_getc();
        if (c == '\r' || c == '\n')
            break;
        *s++ = c;
        ret++;
    }
    *s = '\0';
    ret++;

    return ret;
}

int sk_uart0_puts(char* s) {
    int ret = 0;
    while (*s != '\0') {
        ret += sk_uart0_putc(*s);
    }

    return ret;
}
