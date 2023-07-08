#ifndef _SHELL_H_
#define _SHELL_H_

#include <fat.h>

void callback_ls(DirectoryEntry* entry, void* p);
void cat_file_for_cluster(uint32_t cluster, uint32_t file_size);
uint32_t cluster_for_path(uint32_t current_cluster, const char* path,
                          DirectoryEntry* entry);
void run_shell();

#endif
