#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include "installer.h"
#include "disk.h"
#include "terminal.h"

// Constants
#define SECTOR_SIZE 512
#define ROOT_PARTITION_START 2048  // Start at 1MB boundary
#define SWAP_PARTITION_TYPE 0x82   // Linux swap
#define ROOT_PARTITION_TYPE 0x83   // Linux native

// Minimum disk size in MB
#define MIN_DISK_SIZE 64

// Buffer for disk operations
static uint8_t disk_buffer[512];

// Function declarations
static bool create_partitions(uint8_t drive, uint32_t root_mb, uint32_t swap_mb);
static bool install_bootloader(uint8_t drive);
static bool copy_system_files(uint8_t drive);

// Status messages for installer
static const char* const status_messages[] = {
    "Installation completed successfully",
    "Failed to initialize installer",
    "Disk error occurred",
    "Partition creation failed",
    "Format operation failed",
    "File copy operation failed",
    "Bootloader installation failed"
};

// Implementation of installer functions
bool installer_init(void) {
    // Initialize disk subsystem
    return disk_init(0x80);  // Initialize first hard disk
}

void installer_get_targets(struct install_target* targets, int* count) {
    // Get list of available disks
    disk_get_drives(targets, count);
}

enum install_status installer_install(const struct install_config* config) {
    // Validate configuration
    if (!config) return INSTALL_ERROR_INIT;
    
    // Check minimum disk size
    struct disk_geometry geo;
    if (!disk_get_geometry(config->target_drive, &geo)) {
        return INSTALL_ERROR_DISK;
    }
    
    uint32_t disk_size_mb = (geo.total_sectors * SECTOR_SIZE) / (1024 * 1024);
    if (disk_size_mb < MIN_DISK_SIZE) {
        return INSTALL_ERROR_DISK;
    }
    
    // Validate partition sizes
    if (config->root_size_mb < MIN_ROOT_SIZE || 
        config->root_size_mb > MAX_ROOT_SIZE ||
        config->swap_size_mb > disk_size_mb / 4) {  // Swap shouldn't exceed 25% of disk
        return INSTALL_ERROR_PARTITION;
    }
    
    // Create partitions
    if (!create_partitions(config->target_drive, 
                          config->root_size_mb,
                          config->swap_size_mb)) {
        return INSTALL_ERROR_PARTITION;
    }
    
    // Install bootloader
    if (!install_bootloader(config->target_drive)) {
        return INSTALL_ERROR_BOOTLOADER;
    }
    
    // Copy system files
    if (!copy_system_files(config->target_drive)) {
        return INSTALL_ERROR_COPY;
    }
    
    return INSTALL_SUCCESS;
}

const char* installer_status_message(enum install_status status) {
    if (status >= 0 && status < sizeof(status_messages) / sizeof(status_messages[0])) {
        return status_messages[status];
    }
    return "Unknown error occurred";
}

static bool create_partitions(uint8_t drive, uint32_t root_mb, uint32_t swap_mb) {
    // Calculate sector counts
    uint32_t root_sectors = (root_mb * 1024 * 1024) / SECTOR_SIZE;
    uint32_t swap_sectors = (swap_mb * 1024 * 1024) / SECTOR_SIZE;
    
    // Create root partition
    if (!disk_create_partition(drive, ROOT_PARTITION_START, root_sectors, ROOT_PARTITION_TYPE)) {
        return false;
    }
    
    // Create swap partition
    if (!disk_create_partition(drive, ROOT_PARTITION_START + root_sectors, 
                             swap_sectors, SWAP_PARTITION_TYPE)) {
        return false;
    }
    
    return true;
}

static bool install_bootloader(uint8_t drive) {
    // Read current MBR
    if (!disk_read_sectors(drive, 0, 1, disk_buffer))
        return false;
    
    // Copy our bootloader code (first 446 bytes)
    // In a real implementation, would copy from bootloader binary
    memcpy(disk_buffer, (void*)0x7C00, 446);
    
    // Write back MBR
    return disk_write_sectors(drive, 0, 1, disk_buffer);
}

static bool copy_system_files(uint8_t drive) {
    (void)drive;  // Suppress unused parameter warning
    // In a real implementation, would copy kernel and system files
    // to the root partition. This is a simplified version.
    return true;
} 