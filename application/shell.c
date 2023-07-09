#include "shell.h"
#include "examples.h"
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

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
            printf("\n");
        else
            printf(" ");
    }
    printf("\n");
}

void callback_ls(DirectoryEntry* entry, void* p) {
    char name[13]; // 8 + '.' + 3 + '\0'
    fat_get_entry_name(entry, name, sizeof(name) / sizeof(name[0]));
    if (entry->attributes & 0x10) {
        // Directory
        printf("D %s\n", name);
    } else if (entry->attributes & 0x08) {
        // Volume Label
        printf("V %s\n", name);
    } else {
        // File
        printf("F %s %u\n", name, entry->fileSize);
    }
}

void cat_file_for_cluster(uint32_t cluster, uint32_t file_size) {
    char buffer[1025];
    while (file_size > 0) {
        uint8_t* p = fat_get_cluster_ptr(cluster);
        if (file_size >= 1024) {
            memcpy(buffer, p, 1024);
            buffer[1024] = '\0';
            file_size -= 1024;
            cluster = fat_get_fat(cluster);
        } else {
            memcpy(buffer, p, file_size);
            buffer[file_size] = '\0';
            file_size = 0;
        }
        printf("%s", buffer);
    }
}

uint32_t cluster_for_path(uint32_t current_cluster, const char* path,
                          DirectoryEntry* entry) {
    char* token;
    const char* delim = "/";
    char tmp_path[64];
    strcpy(tmp_path, path);

    token = strtok(tmp_path, delim);
    uint32_t prev_cluster;
    uint32_t cluster = current_cluster;
    while (token) {
        prev_cluster = cluster;
        fat_set_entry_name(entry, token);
        cluster = fat_get_cluster_for_entry(prev_cluster, entry);
        token = strtok(NULL, delim);
    }
    return cluster;
}

void process_command(char* cmd) {
    printf("\n");
    const char* delim = " ";
    char* token;
    token = strtok(cmd, delim);
    uint32_t cluster = 0;
    DirectoryEntry entry;

    if (strcmp(token, "help") == 0) {
        printf("whoami: show user\n");
        printf("version: show version\n");
        printf("fat: show FAT FS info\n");
        printf("lcd: show stack info in LCD\n");
        printf("ls\n");
        printf("cat\n");
        printf("pwd\n");
        printf("cd\n");
        printf("uptime: show uptime\n");
        printf("memread [address] [bytes]: show memory\n");
        printf("\n");
    } else if (strcmp(token, "whoami") == 0) {
        printf("root\n");
        printf("root\n");
    } else if (strcmp(token, "version") == 0) {
        printf("sokoide os ver: %s\n", VERSION);
    } else if (strcmp(cmd, "echo") == 0) {
        // TODO: no quote handling
        printf("%s\n", &cmd[5]);
    } else if (strcmp(token, "fat") == 0) {
        fat_print_info();
        fat_print_header_legend();
        fat_print_header_dump();
    } else if (strcmp(token, "lcd") == 0) {
        device();
    } else if (strcmp(token, "ls") == 0) {
        token = strtok(NULL, delim);
        if (token != NULL) {
            cluster = cluster_for_path(0, token, &entry);
        }
        if (token != NULL && cluster == 0) {
            printf("cluster not found\n");
            return;
        }
        iterate_dir(cluster, callback_ls, NULL);
    } else if (strcmp(token, "cat") == 0) {
        token = strtok(NULL, delim);
        if (token != NULL) {
            cluster = cluster_for_path(0, token, &entry);
        }
        if (token != NULL && cluster == 0) {
            printf("cluster not found\n");
            return;
        }
        cat_file_for_cluster(cluster, entry.fileSize);
    } else if (strcmp(token, "pwd") == 0) {
        // TODO: no file system supported yet
        printf("/\n");
    } else if (strncmp(token, "cd ", 3) == 0) {
        // TODO: no file system supported yet
        printf("cd not supported yet\n");
    } else if (strcmp(token, "uptime") == 0) {
        time_t t;
        time(&t);
        printf("up: %lld seconds\n", t);
    } else if (strncmp(token, "memread ", 7) == 0) {
        uintptr_t addr;
        int bytes;
        parse_memread_args(&cmd[8], &addr, &bytes);
        memread(addr, bytes);
    } else {
        printf("unknown command: %s\n", cmd);
    }
    fflush(stdout);
}

void run_shell() {
    char line[32];
    fat_init();

    while (1) {
        printf("> ");
        fflush(stdout);

        sk_delay_task(1);

        sk_uart0_gets_s(line, sizeof(line) / sizeof(char));
        process_command(line);

        fflush(stdout);
    }
}
