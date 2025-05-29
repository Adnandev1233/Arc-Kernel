#ifndef DISK_H
#define DISK_H

#include <stdint.h>
#include <stdbool.h>

// Disk types
#define DISK_TYPE_FLOPPY    0
#define DISK_TYPE_HARDDISK  1

// Disk geometry structure
typedef struct {
    uint16_t cylinders;
    uint8_t heads;
    uint8_t sectors;
} disk_geometry_t;

// Disk drive structure
typedef struct {
    uint8_t type;      // DISK_TYPE_*
    uint8_t number;    // Drive number
} disk_drive_t;

// Initialize disk
bool disk_init(void);

// Get disk geometry
bool disk_get_geometry(uint8_t drive, disk_geometry_t* geometry);

// Read sectors from disk
bool disk_read_sectors(uint8_t drive, uint32_t lba, uint8_t sectors, void* buffer);

// Write sectors to disk
bool disk_write_sectors(uint8_t drive, uint32_t lba, uint8_t sectors, const void* buffer);

// Create partition
bool disk_create_partition(uint8_t drive, uint32_t start_lba, uint32_t size_sectors, uint8_t type);

// Get available drives
void disk_get_drives(disk_drive_t* drives, int* count);

#endif // DISK_H 