#include <kernel.h>
#include <stdio.h>
#include <string.h>

// offset: 10 for decimal, 16 for hex
uintptr_t str2int(char* p, int offset) {
    uintptr_t i = 0;
    while (*p != '\n' && *p != '\r' && *p != '\0') {
        i = i * offset;
        if (*p >= 'a' && *p <= 'f') {
            i += *p - 'a' + 10;
        } else if (*p >= 'A' && *p <= 'F') {
            i += *p - 'A' + 10;
        } else {
            i += *p - '0';
        }
        p++;
    }
    return i;
}

void parse_memread_args(char* p, uintptr_t* addr, int* bytes) {
    char* tok = strtok(p, " ");
    *addr = str2int(tok, 16);
    tok = strtok(NULL, " ");
    if (tok == NULL)
        *bytes = 8;
    else
        *bytes = str2int(tok, 10);
}

void memread(uintptr_t addr, int bytes) {
    int printed = 0;
    while (printed < bytes) {
        uint32_t data = in_w(addr);
        if (printed % 8 == 0)
            printf("0x%08X: ", addr);
        for (int i = 0; i < 4; i++) {
            uint8_t d8 = data & 0xff;
            printf("%02X", d8);
            data = data >> 4;
            printed++;
        }
        addr += 4;
        if (printed % 8 == 0)
            printf("\r\n");
        else
            printf(" ");
    }
    printf("\r\n");
}

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
    } else if (strncmp(cmd, "memread ", 7) == 0) {
        uintptr_t addr;
        int bytes;
        parse_memread_args(&cmd[8], &addr, &bytes);
        memread(addr, bytes);
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

        sk_delay_task(1);

        sk_uart0_gets_s(line, sizeof(line) / sizeof(char));
        process_command(line);

        fflush(stdout);
    }
}
