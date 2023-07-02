#include <kernel.h>
#include <stdio.h>

void run_shell() {
    while (1) {
        printf("> ");
        fflush(stdout);

        char line[16];
        sk_delay_task(1000);

        sk_uart0_gets_s(line, sizeof(line) / sizeof(char));
        printf("line: %s\r\n", line);
        fflush(stdout);
    }
}
