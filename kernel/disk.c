#include <stddef.h>
#include <string.h>
#include <io.h>
#include "disk.h"

// Define inline assembly macros
#ifdef __GNUC__
    #define ASM_VOLATILE(...) __asm__ __volatile__(__VA_ARGS__)
#else
    #error "This code requires GCC for inline assembly support"
#endif

// BIOS disk functions
#define BIOS_READ_SECTORS    0x02
#define BIOS_WRITE_SECTORS   0x03
#define BIOS_GET_PARAMETERS  0x08

// Error codes
#define DISK_NO_ERROR        0x00
#define DISK_ERROR_GENERAL   0x01
#define DISK_ERROR_NOTREADY  0x80

// Maximum retries for disk operations
#define MAX_RETRIES         3

// Convert LBA to CHS
static void lba_to_chs(uint32_t lba, const struct disk_geometry* geo, 
                      uint8_t* cyl, uint8_t* head, uint8_t* sect) {
    *sect = (lba % geo->sectors) + 1;
    lba /= geo->sectors;
    *head = lba % geo->heads;
    *cyl = lba / geo->heads;
}

bool disk_init(uint8_t drive) {
    struct disk_geometry geo;
    bool success = false;
    int retries = MAX_RETRIES;
    
    while (retries-- && !success) {
        // Reset disk controller
        ASM_VOLATILE("int $0x13" : : "a"(0), "d"(drive));
        
        // Try to get disk parameters
        success = disk_get_geometry(drive, &geo);
        
        if (success) {
            // Verify disk is ready by reading first sector
            uint8_t buffer[512];
            success = disk_read_sectors(drive, 0, 1, buffer);
        }
    }
    
    return success;
}

bool disk_get_geometry(uint8_t drive, struct disk_geometry* geometry) {
    uint16_t ax, bx, cx, dx;
    int retries = MAX_RETRIES;
    bool success = false;
    
    while (retries-- && !success) {
        // Call BIOS to get disk parameters
        ASM_VOLATILE("int $0x13"
                    : "=a"(ax), "=b"(bx), "=c"(cx), "=d"(dx)
                    : "a"(0x0800), "d"(drive)
                    : "memory");
        
        if (!(ax & 0xFF00)) {  // No error
            // Extract geometry from registers
            geometry->heads = ((dx >> 8) & 0xFF) + 1;
            geometry->sectors = cx & 0x3F;
            geometry->cylinders = ((cx >> 8) & 0xFF) | ((cx & 0xC0) << 2);
            geometry->total_sectors = geometry->cylinders * geometry->heads * geometry->sectors;
            success = true;
        } else {
            // Reset disk controller and try again
            ASM_VOLATILE("int $0x13" : : "a"(0), "d"(drive));
        }
    }
    
    return success;
}

bool disk_read_sectors(uint8_t drive, uint32_t lba, uint8_t sectors, void* buffer) {
    struct disk_geometry geo;
    uint8_t cylinder, head, sector;
    uint16_t ax, flags;
    int retries = MAX_RETRIES;
    bool success = false;
    
    if (!disk_get_geometry(drive, &geo)) return false;
    lba_to_chs(lba, &geo, &cylinder, &head, &sector);
    
    while (retries-- && !success) {
        // Call BIOS to read sectors
        ASM_VOLATILE("int $0x13"
                    : "=a"(ax), "=@ccnc"(flags)
                    : "a"((BIOS_READ_SECTORS << 8) | sectors),
                      "b"(buffer),
                      "c"(((cylinder & 0xFF) << 8) | sector),
                      "d"((head << 8) | drive)
                    : "memory");
        
        if (flags) {
            success = true;
        } else {
            // Reset disk system and retry
            ASM_VOLATILE("int $0x13" : : "a"(0), "d"(drive));
        }
    }
    
    return success;
}

bool disk_write_sectors(uint8_t drive, uint32_t lba, uint8_t sectors, const void* buffer) {
    struct disk_geometry geo;
    uint8_t cylinder, head, sector;
    uint16_t ax, flags;
    int retries = MAX_RETRIES;
    bool success = false;
    
    if (!disk_get_geometry(drive, &geo)) return false;
    lba_to_chs(lba, &geo, &cylinder, &head, &sector);
    
    while (retries-- && !success) {
        // Call BIOS to write sectors
        ASM_VOLATILE("int $0x13"
                    : "=a"(ax), "=@ccnc"(flags)
                    : "a"((BIOS_WRITE_SECTORS << 8) | sectors),
                      "b"(buffer),
                      "c"(((cylinder & 0xFF) << 8) | sector),
                      "d"((head << 8) | drive)
                    : "memory");
        
        if (flags) {
            success = true;
        } else {
            // Reset disk system and retry
            ASM_VOLATILE("int $0x13" : : "a"(0), "d"(drive));
        }
    }
    
    return success;
}

bool disk_create_partition(uint8_t drive, uint32_t start_lba, uint32_t size_sectors, uint8_t type) {
    struct disk_geometry geo;
    struct partition_entry* part;
    uint8_t mbr[512];
    uint8_t start_cyl, start_head, start_sect;
    uint8_t end_cyl, end_head, end_sect;
    
    // Read current MBR
    if (!disk_read_sectors(drive, 0, 1, mbr)) return false;
    
    // Get disk geometry
    if (!disk_get_geometry(drive, &geo)) return false;
    
    // Convert start LBA to CHS
    lba_to_chs(start_lba, &geo, &start_cyl, &start_head, &start_sect);
    
    // Convert end LBA to CHS
    lba_to_chs(start_lba + size_sectors - 1, &geo, &end_cyl, &end_head, &end_sect);
    
    // Find first empty partition entry
    part = (struct partition_entry*)(mbr + 0x1BE);
    for (int i = 0; i < 4; i++, part++) {
        if (part->system_id == PART_TYPE_EMPTY) {
            // Fill partition entry
            part->bootable = 0;
            part->start_head = start_head;
            part->start_sector = start_sect;
            part->start_cyl_high = (start_cyl >> 8) & 0x03;
            part->start_cyl_low = start_cyl & 0xFF;
            part->system_id = type;
            part->end_head = end_head;
            part->end_sector = end_sect;
            part->end_cyl_high = (end_cyl >> 8) & 0x03;
            part->end_cyl_low = end_cyl & 0xFF;
            part->start_lba = start_lba;
            part->sector_count = size_sectors;
            
            // Write updated MBR
            return disk_write_sectors(drive, 0, 1, mbr);
        }
    }
    
    return false;  // No empty partition slots
}

void disk_get_drives(struct install_target* targets, int* count) {
    struct disk_geometry geo;
    int found = 0;
    
    // Try first four hard disks
    for (uint8_t drive = 0x80; drive < 0x84 && found < *count; drive++) {
        if (disk_get_geometry(drive, &geo)) {
            targets[found].drive = drive;
            targets[found].size_mb = (geo.total_sectors * 512) / (1024 * 1024);
            strcpy(targets[found].model, "Hard Disk");  // Basic model name
            found++;
        }
    }
    
    *count = found;
} 