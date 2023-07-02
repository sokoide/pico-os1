#include <kernel.h>
#include <stdio.h>
#include <string.h>

void process_command(char* cmd) {
    printf("\r\n");
    if (strcmp(cmd, "whoami") == 0) {
        printf("root\r\n");
    } else if (strcmp(cmd, "version") == 0) {
        printf("sokoide os ver: %s\r\n", VERSION);
    } else if (strncmp(cmd, "echo ", 5) == 0) {
        // TODO: no quote handling
        printf("%s\r\n", &cmd[5]);
    } else if (strcmp(cmd, "pwd") == 0) {
        // TODO: no file system supported yet
        printf("/\r\n");
    } else if (strncmp(cmd, "cd ", 3) == 0) {
        // TODO: no file system supported yet
    } else {
        printf("unknown command: %s\r\n", cmd);
    }
    fflush(stdout);
}

void run_shell() {
    char line[32];
    while (1) {
        printf("> ");
        fflush(stdout);

        sk_delay_task(1000);

        sk_uart0_gets_s(line, sizeof(line) / sizeof(char));
        process_command(line);

        fflush(stdout);
    }
}
