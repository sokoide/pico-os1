#include <kernel.h>

void tm_com_init(void) {
    // https://github.com/raspberrypi/pico-sdk/blob/master/src/rp2_common/hardware_uart/uart.c#L128C1-L155C1
    // set baudrate
    out_w(UART0_BASE + UARTx_IBRD, 67);
    out_w(UART0_BASE + UARTx_FBRD, 52);
    // set format
    out_w(UART0_BASE + UARTx_LCR_H, 0x70);
    // enable UART
    out_w(UART0_BASE + UARTx_CR, UART_CR_RXE | UART_CR_TXE | UART_CR_EN);
}


