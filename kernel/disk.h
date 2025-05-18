#ifndef DISK_H
#define DISK_H

#include <stdint.h>
#include <stdbool.h>
#include "installer.h"  // For struct install_target

// Compiler-specific definitions
#ifdef __GNUC__
    #define PACKED __attribute__((packed))
#else
    #define PACKED
#endif

// Memory management constants
#define MAX_PAGES 1024
#define PAGE_SIZE 4096

// Disk geometry structure
struct disk_geometry {
    uint16_t cylinders;
    uint8_t heads;
    uint8_t sectors;
    uint32_t total_sectors;
};

// Partition table entry structure
struct partition_entry {
    uint8_t bootable;           // 0x80 = bootable, 0x00 = not bootable
    uint8_t start_head;
    uint8_t start_sector : 6;   // Bits 0-5: sector, Bits 6-7: high bits of cylinder
    uint8_t start_cyl_high : 2;
    uint8_t start_cyl_low;
    uint8_t system_id;          // Partition type
    uint8_t end_head;
    uint8_t end_sector : 6;     // Bits 0-5: sector, Bits 6-7: high bits of cylinder
    uint8_t end_cyl_high : 2;
    uint8_t end_cyl_low;
    uint32_t start_lba;         // LBA of first sector
    uint32_t sector_count;      // Number of sectors
} PACKED;

// Disk operations
bool disk_init(uint8_t drive);
bool disk_read_sectors(uint8_t drive, uint32_t lba, uint8_t sectors, void* buffer);
bool disk_write_sectors(uint8_t drive, uint32_t lba, uint8_t sectors, const void* buffer);
bool disk_get_geometry(uint8_t drive, struct disk_geometry* geometry);
bool disk_create_partition(uint8_t drive, uint32_t start_lba, uint32_t size_sectors, uint8_t type);
void disk_get_drives(struct install_target* targets, int* count);

// Partition types
#define PART_TYPE_EMPTY     0x00
#define PART_TYPE_FAT12     0x01
#define PART_TYPE_FAT16     0x04
#define PART_TYPE_EXTENDED  0x05
#define PART_TYPE_FAT32     0x0B
#define PART_TYPE_LINUX     0x83
#define PART_TYPE_SWAP      0x82

// Disk buffer location (after bootloader)
#define DISK_BUFFER_ADDR 0x8000

#endif /* DISK_H */ 