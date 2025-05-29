#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <io.h>
#include "disk.h"
#include "bios.h"

// BIOS disk functions
#define BIOS_DISK_READ_SECTORS  0x02
#define BIOS_DISK_WRITE_SECTORS 0x03
#define BIOS_DISK_GET_PARAMS   0x08

// Error codes
#define DISK_ERROR_NONE        0x00
#define DISK_ERROR_BAD_COMMAND 0x01
#define DISK_ERROR_BAD_SECTOR  0x02
#define DISK_ERROR_WRITE_FAULT 0x03
#define DISK_ERROR_READ_FAULT  0x04
#define DISK_ERROR_GENERAL     0x05

// Maximum retries
#define MAX_RETRIES 3

// Convert LBA to CHS
static void lba_to_chs(uint32_t lba, uint16_t* cylinder, uint8_t* head, uint8_t* sector) {
    *sector = (lba % 63) + 1;
    *head = ((lba / 63) % 255);
    *cylinder = (lba / (63 * 255));
}

// Initialize disk
bool disk_init(void) {
    uint16_t ax, bx, cx, dx;
    
    // Get disk parameters
    if (!bios_call(BIOS_DISK_GET_PARAMS, 0, 0, 0, 0, &ax, &bx, &cx, &dx)) {
        return false;
    }
    
    return true;
}

// Get disk geometry
bool disk_get_geometry(uint8_t drive, disk_geometry_t* geometry) {
    uint16_t ax, bx, cx, dx;
    
    // Get disk parameters
    if (!bios_call(BIOS_DISK_GET_PARAMS, drive, 0, 0, 0, &ax, &bx, &cx, &dx)) {
        return false;
    }
    
    // Extract geometry information
    geometry->cylinders = ((cx & 0xC0) << 2) | (cx & 0xFF);
    geometry->heads = dx & 0xFF;
    geometry->sectors = cx & 0x3F;
    
    return true;
}

// Read sectors from disk
bool disk_read_sectors(uint8_t drive, uint32_t lba, uint8_t sectors, void* buffer) {
    uint16_t ax, bx, cx, dx;
    uint16_t cylinder;
    uint8_t head, sector;
    uint8_t retries = 0;
    
    // Convert LBA to CHS
    lba_to_chs(lba, &cylinder, &head, &sector);
    
    // Prepare parameters
    ax = (sectors << 8) | BIOS_DISK_READ_SECTORS;
    bx = (uint32_t)buffer & 0xFFFF;  // Use lower 16 bits of buffer address
    cx = (cylinder << 8) | sector;
    dx = (drive << 8) | head;
    
    // Try to read sectors
    while (retries < MAX_RETRIES) {
        if (bios_call_with_buffer(0x13, ax, bx, cx, dx, buffer, &ax)) {
            return true;
        }
        retries++;
    }
    
    return false;
}

// Write sectors to disk
bool disk_write_sectors(uint8_t drive, uint32_t lba, uint8_t sectors, const void* buffer) {
    uint16_t ax, bx, cx, dx;
    uint16_t cylinder;
    uint8_t head, sector;
    uint8_t retries = 0;
    
    // Convert LBA to CHS
    lba_to_chs(lba, &cylinder, &head, &sector);
    
    // Prepare parameters
    ax = (sectors << 8) | BIOS_DISK_WRITE_SECTORS;
    bx = (uint32_t)buffer & 0xFFFF;  // Use lower 16 bits of buffer address
    cx = (cylinder << 8) | sector;
    dx = (drive << 8) | head;
    
    // Try to write sectors
    while (retries < MAX_RETRIES) {
        if (bios_call_with_buffer(0x13, ax, bx, cx, dx, (void*)buffer, &ax)) {
            return true;
        }
        retries++;
    }
    
    return false;
}

// Create partition
bool disk_create_partition(uint8_t drive, uint32_t start_lba, uint32_t size_sectors, uint8_t type) {
    (void)drive;        // Unused parameter
    (void)start_lba;    // Unused parameter
    (void)size_sectors; // Unused parameter
    (void)type;         // Unused parameter
    
    // TODO: Implement partition creation
    return false;
}

// Get available drives
void disk_get_drives(disk_drive_t* drives, int* count) {
    uint16_t ax, bx, cx, dx;
    int drive_count = 0;
    
    // Check for floppy drives
    if (bios_call(BIOS_DISK_GET_PARAMS, 0, 0, 0, 0, &ax, &bx, &cx, &dx)) {
        drives[drive_count].type = DISK_TYPE_FLOPPY;
        drives[drive_count].number = 0;
        drive_count++;
    }
    
    // Check for hard drives
    if (bios_call(BIOS_DISK_GET_PARAMS, 0x80, 0, 0, 0, &ax, &bx, &cx, &dx)) {
        drives[drive_count].type = DISK_TYPE_HARDDISK;
        drives[drive_count].number = 0x80;
        drive_count++;
    }
    
    *count = drive_count;
} 