#ifndef FS_H
#define FS_H

#include <stdint.h>
#include <stdbool.h>

// Filesystem types
#define FS_TYPE_NONE  0
#define FS_TYPE_FAT32 1
#define FS_TYPE_EXT2  2

// Filesystem status
#define FS_STATUS_OK    0
#define FS_STATUS_ERROR 1

// Basic filesystem structure
typedef struct {
    uint8_t type;
    uint8_t status;
    uint32_t total_size;    // Total size in bytes
    uint32_t free_size;     // Free space in bytes
    char label[32];         // Volume label
} fs_t;

// Function declarations
bool fs_init(void);
bool fs_mount(const char* device);
bool fs_umount(void);
fs_t* fs_get_info(void);

#endif // FS_H 