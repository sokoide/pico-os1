#include "fat.h"
#include <assert.h>
#include <ctype.h>
#include <memory.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// globals

// static vars
void* _fat_buffer;
FatBS* _fat_bs;
static enum FAT_TYPE _fat_type;
static uint32_t _fat_start_sector;
static uint32_t _fat_sectors;
static uint32_t _root_dir_start_sector;
static uint32_t _root_dir_sectors;
static uint32_t _data_start_sector;

// functions
bool fat_init() {
    _fat_buffer = (void*)0x10100000;
    _fat_bs = _fat_buffer;

    // init static vars
    _fat_start_sector = _fat_bs->reservedSectorCount;
    _fat_sectors = _fat_bs->tableSize16 * _fat_bs->tableCount;
    _root_dir_start_sector = _fat_start_sector + _fat_sectors;
    _root_dir_sectors = (sizeof(DirectoryEntry) * _fat_bs->rootEntryCount +
                         _fat_bs->bytesPerSector - 1) /
                        _fat_bs->bytesPerSector;
    _data_start_sector = _root_dir_start_sector + _root_dir_sectors;
    int clusters = (_fat_bs->totalSectors16 - _data_start_sector) /
                   _fat_bs->sectorsPerCluster;
    if (clusters <= 4085)
        _fat_type = FT_FAT12;
    else if (clusters <= 65525)
        _fat_type = FT_FAT16;
    else
        _fat_type = FT_FAT32;

    return true;
}

void fat_unint() {}

void fat_print_info() {
    printf("bytesPerSector: %d\n", _fat_bs->bytesPerSector);
    printf("sectorsPerCluster: %d\n", _fat_bs->sectorsPerCluster);
    // 1st FAT table's sector
    printf("reservedSectorCount: %d\n", _fat_bs->reservedSectorCount);
    // count of FAT tables
    printf("tableCount: %d\n", _fat_bs->tableCount);
    printf("rootEntryCount: %d\n", _fat_bs->rootEntryCount);
    printf("root dir sector count: %d\n", _fat_bs->rootEntryCount *
                                              sizeof(DirectoryEntry) /
                                              _fat_bs->bytesPerSector);
    printf("total_sectors: %d\n", _fat_bs->totalSectors16);
    // count of FAT table sectors
    printf("tableSize16: %d\n", _fat_bs->tableSize16);

    printf("* fat start_sector %d\n", _fat_start_sector);
    printf("* fat sectors %d\n", _fat_sectors);
    printf("* root_dir start_sector %d\n", _root_dir_start_sector);
    printf("* root_dir sectors %d\n", _root_dir_sectors);
    printf("* data start_sector %d\n", _data_start_sector);
}

void fat_print_header_legend() {
    fat_print_legend("relative jump (eb3c) + nop (90)");
    fat_print_legend("OEM Name");
    fat_print_legend("bytes per sector");
    fat_print_legend("sectors per cluster");
    fat_print_legend("FAT table's 1st sector (reserved sectors)");
    fat_print_legend("FAT table count");
    fat_print_legend("Max entries in root table");
    fat_print_legend("Total sector count");
    fat_print_legend("Media type");
    fat_print_legend("sectors per FAT table");
    fat_print_legend("sectors per track");
    fat_print_legend("head count");
}

void fat_print_header_dump() {
    // 1st line
    int idx = 0;
    const int l[] = {3, 8, 2, 1, 2, -1};
    fat_print_idx_wide((uint8_t*)_fat_bs, &idx, l);
    printf("\n");

    // 2nd line
    idx = 16;
    const int l2[] = {1, 2, 2, 1, 2, 2, 2, 4, -1};
    fat_print_idx_wide((uint8_t*)_fat_bs, &idx, l2);
    printf("\n");
}

void fat_print_legend(const char* legend) { printf("- %s\n", legend); }

void fat_print_idx_wide(const uint8_t* base, int* idx, const int* lens) {
    int idxStr = *idx;
    for (int i = 0; lens[i] > 0; i++) {
        fat_print_idx((uint8_t*)base, idx, lens[i]);
    }

    for (int i = 0; lens[i] > 0; i++) {
        fat_print_idxstr(base, &idxStr, lens[i]);
    }
}

void fat_print_idx(const uint8_t* base, int* idx, const int len) {
    for (int i = 0; i < len; i++) {
        printf("%02x ", (base)[*idx + i]);
    }
    *idx = *idx + len;
}

void fat_print_idxstr(const void* base, int* idxStr, const int len) {
    for (int i = 0; i < len; i++) {
        uint8_t u = *(uint8_t*)(base + *idxStr + i);
        if (40 <= u && u <= 126)
            printf("%c ", u);
        else
            printf("**");
    }
    *idxStr = *idxStr + len;
}

void fat_print_bs_idx(int* idx, int len) {
    fat_print_idx((uint8_t*)_fat_bs, idx, len);
}

void fat_print_fat12() {
    // olny print the 1st FAT table
    for (int i = 0; i < _fat_bs->tableCount * _fat_bs->bytesPerSector / 3;
         i++) {
        uint32_t value = fat_get_fat(i);
        printf("%03X ", value);
        if (i % 10 == 9)
            printf("\n");
    }
    printf("\n");
}

void fat_print_directory_entry_header_legend() {
    fat_print_legend("Name");
    fat_print_legend("Attributes");
    fat_print_legend("(reserved)");
    fat_print_legend("Creation time (millisec)");
    fat_print_legend("Creation time");
    fat_print_legend("Creation date");
    fat_print_legend("Last access date");
    fat_print_legend("(ignored in FAT12)");
    fat_print_legend("Last write time");
    fat_print_legend("Last write date");
    fat_print_legend("Starting cluster");
    fat_print_legend("File size");
}

void fat_print_directory_entry_dump(DirectoryEntry* entry, void* p) {
    // 1st line
    int idx = 0;
    const int l1[] = {11, 1, 1, 1, 2, -1};
    const int l2[] = {2, 2, 2, 2, 2, 2, 4, -1};
    fat_print_idx_wide((uint8_t*)entry, &idx, l1);
    printf("\n");
    // 2nd line
    fat_print_idx_wide((uint8_t*)entry, &idx, l2);
    printf("\n");
}

void fat_print_directory_entry(DirectoryEntry* entry) {
    // Check if entry is unused or deleted
    if (entry->name[0] == 0x00 || entry->name[0] == 0xE5)
        return;

    // Check if entry is a directory
    if (entry->attributes & 0x10) {
        // Directory
        if (entry->name[0] != '.') {
            fat_print_directory_entry_directory(entry, true);
        }
    } else {
        // File entry
        fat_print_directory_entry_file(entry);
    }
}

void fat_print_directory_entry_directory(DirectoryEntry* entry,
                                         bool recursive) {
    // the entry must be a directory
    char directoryName[13];
    memcpy(directoryName, entry->name, 11);
    directoryName[11] = '\0';

    printf("Directory: %s, cluster:%d[0x%08X]\n", directoryName,
           entry->startingClusterNumber,
           fat_get_cluster_addr(entry->startingClusterNumber));
    fat_print_directory_entry_dump(entry, NULL);

    if (recursive && entry->startingClusterNumber > 0) {
        DirectoryEntry* subEntries =
            (DirectoryEntry*)fat_get_cluster_ptr(entry->startingClusterNumber);
        if (entry->startingClusterNumber == 0) {
            // root directory
            for (uint32_t i = 0;
                 i < _fat_bs->bytesPerSector * _fat_bs->sectorsPerCluster /
                         sizeof(DirectoryEntry);
                 i++) {
                if (subEntries[i].name[0] == 0x00)
                    break;
                else if (subEntries[i].name[0] == '.')
                    continue;
                fat_print_directory_entry(&subEntries[i]);
            }
        } else {
            // non-root directory
            uint32_t cluster = entry->startingClusterNumber;
            uint32_t nextCluster = fat_get_fat(cluster);
            DirectoryEntry* directoryEntries;

            while (cluster < 0xF00) {
                nextCluster = fat_get_fat(cluster);
                directoryEntries =
                    (DirectoryEntry*)(fat_get_cluster_ptr(cluster));
                for (uint32_t i = 0;
                     i < _fat_bs->bytesPerSector * _fat_bs->sectorsPerCluster /
                             sizeof(DirectoryEntry);
                     i++) {
                    DirectoryEntry* entry = &directoryEntries[i];
                    // 0x00 not use, 0xE5 deleted
                    if (entry->name[0] == 0x00 || entry->name[0] == 0xE5)
                        break;
                    fat_print_directory_entry(&directoryEntries[i]);
                }
                cluster = nextCluster;
            }
        }
    }
}

void fat_print_directory_entry_file(DirectoryEntry* entry) {
    // the entry must be a file
    char fileName[13];
    memcpy(fileName, entry->name, 11);
    fileName[11] = '\0';

    printf("File: %s, cluster:%d[0x%08X],  size:%d\n", fileName,
           entry->startingClusterNumber,
           fat_get_cluster_addr(entry->startingClusterNumber), entry->fileSize);
    fat_print_directory_entry_dump(entry, NULL);
}

void iterate_rootdir(iterate_dir_callback callback, void* p) {
    DirectoryEntry* directoryEntries;
    FatBS* bs = (FatBS*)fat_get_ptr();

    directoryEntries =
        (DirectoryEntry*)fat_get_root_directory_start_sector_ptr();

    for (int i = 0; i < bs->rootEntryCount; i++) {
        DirectoryEntry* entry = &directoryEntries[i];
        // 0x00 not use, 0xE5 deleted
        if (entry->name[0] == 0x00 || entry->name[0] == 0xE5)
            break;
        if (callback != NULL) {
            callback(entry, p);
        }
    }
}

void iterate_dir(uint32_t cluster, iterate_dir_callback callback, void* p) {
    uint32_t nextCluster;
    DirectoryEntry* directoryEntries;
    FatBS* bs = (FatBS*)fat_get_ptr();

    if (cluster == 0)
        return iterate_rootdir(callback, p);

    nextCluster = fat_get_fat(cluster);

    while (cluster < 0xF00) {
        nextCluster = fat_get_fat(cluster);
        directoryEntries = (DirectoryEntry*)(fat_get_cluster_ptr(cluster));
        for (uint32_t i = 0; i < bs->bytesPerSector * bs->sectorsPerCluster /
                                     sizeof(DirectoryEntry);
             i++) {
            DirectoryEntry* entry = &directoryEntries[i];
            // 0x00 not use, 0xE5 deleted
            if (entry->name[0] == 0x00 || entry->name[0] == 0xE5)
                break;
            if (callback != NULL) {
                callback(entry, p);
            }
        }
        cluster = nextCluster;
    }
}

char* fat_get_entry_name(DirectoryEntry* entry, char* name, int len) {
    if (len < 12) {
        fprintf(stderr, "len must be >=12");
        return NULL;
    }
    memcpy(name, (const char*)(entry->name), 8);
    int i;
    for (i = 0; i < 8; i++) {
        if (name[i] == 0x20)
            break;
    }
    if ((entry->attributes & 0x18) == 0) {
        // not Directory or Volume
        name[i++] = '.';
        memcpy(&name[i], (const char*)&(entry->name[8]), 3);
        i += 3;
    }
    name[i] = '\0';
    return name;
}

void fat_set_entry_name(DirectoryEntry* entry, const char* name) {
    int name_len = strlen(name);
    int dotPos = -1;
    int i = 0;
    for (i = 0; i < name_len; i++) {
        if (name[i] == '.') {
            dotPos = i;
            break;
        }
        entry->name[i] = name[i];
    }
    while (i < 11) {
        entry->name[i] = 0x20;
        i++;
    }

    i = 8;
    if (dotPos >= 0) {
        for (int j = dotPos + 1; j < name_len; j++) {
            entry->name[i++] = name[j];
        }
    }

    // to upper
    for (int i = 0; i < 11; i++) {
        if ('a' <= entry->name[i] && entry->name[i] <= 'z') {
            entry->name[i] = entry->name[i] - 'a' + 'A';
        }
    }
}

void* fat_get_ptr() { return _fat_buffer; }

enum FAT_TYPE fat_get_type() { return _fat_type; }

void* fat_get_sector_ptr(int sector) {
    void* addr = _fat_buffer + sector * _fat_bs->bytesPerSector;
    return addr;
}

void* fat_get_root_directory_start_sector_ptr() {
    return fat_get_sector_ptr(_root_dir_start_sector);
}

uint32_t fat_get_fat(uint32_t cluster) {
    uint8_t* fatp = (uint8_t*)fat_get_sector_ptr(_fat_bs->reservedSectorCount);
    uint32_t value = 0;
    if (cluster % 2 == 0) {
        value = fatp[cluster / 2 * 3];
        value |= ((fatp[cluster / 2 * 3 + 1] & 0x0F) << 8);
    } else {
        value = fatp[cluster / 2 * 3 + 2];
        value = value << 4;
        value |= ((fatp[cluster / 2 * 3 + 1] & 0xF0) >> 4);
    }
    return value;
}

bool fat_is_broken(int cluster) {
    // assume FAT12, FF7: broken cluster, FF8-FFF: end of cluster chain
    // FAT16, FFF7: broken, FFF8-FFFF: end of cluster chain
    // FAT32, FFFFFFF7: broken, FFFFFFF8-FFFFFFFF: end of cluster chain
    uint32_t fat = fat_get_fat(cluster);
    // assume FAT12
    switch (_fat_type) {
    case FT_FAT12:
        return fat == 0xFF7;
    case FT_FAT16:
        return fat == 0xFFF7;
    case FT_FAT32:
        return fat == 0xFFFFFFF7;
    case FT_UNKNOWN:
        assert(false);
        return true;
    }
}

bool fat_is_end_of_cluster(int cluster) {
    uint32_t fat = fat_get_fat(cluster);
    switch (_fat_type) {
    case FT_FAT12:
        return (fat > 0xFF7) ? true : false;
    case FT_FAT16:
        return (fat > 0xFFF7) ? true : false;
    case FT_FAT32:
        return (fat > 0xFFFFFFF7) ? true : false;
    case FT_UNKNOWN:
        assert(false);
        return true;
    }
}

uint32_t fat_get_cluster_addr(int cluster) {
    return (_data_start_sector + (cluster - 2) * _fat_bs->sectorsPerCluster) *
           _fat_bs->bytesPerSector;
}

void* fat_get_cluster_ptr(int cluster) {
    return _fat_buffer + fat_get_cluster_addr(cluster);
}

typedef struct {
    DirectoryEntry* entry;
    /* unsigned char name[11]; */
    bool found;
    /* uint32_t cluster; */
} CallbackFindEntryArg;

void _callback_find_entry(DirectoryEntry* entry, void* p) {
    CallbackFindEntryArg* arg = (CallbackFindEntryArg*)p;
    if (memcmp(arg->entry->name, entry->name, 11) == 0) {
        arg->found = true;
        arg->entry->fileSize = entry->fileSize;
        arg->entry->attributes = entry->attributes;
        arg->entry->creationDate = entry->creationDate;
        arg->entry->creationTime = entry->creationTime;
        arg->entry->creationTimeTenthOfSecond =
            entry->creationTimeTenthOfSecond;
        arg->entry->lastWriteTime = entry->lastWriteTime;
        arg->entry->lastWriteDate = entry->lastWriteDate;
        arg->entry->lastAccessDate = entry->lastAccessDate;
        arg->entry->startingClusterNumber = entry->startingClusterNumber;
    }
}

uint32_t fat_get_cluster_for_entry(uint32_t parent_cluster,
                                   DirectoryEntry* entry) {
    if (strlen((char*)entry->name) == 0 || entry->name[0] == 0x20)
        return 0;

    CallbackFindEntryArg arg;
    arg.entry = entry;
    arg.found = false;

    iterate_dir(parent_cluster, _callback_find_entry, &arg);
    if (arg.found) {
        return arg.entry->startingClusterNumber;
    }
    return 0;
}
