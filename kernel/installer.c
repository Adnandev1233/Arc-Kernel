#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include "include/installer.h"
#include "include/terminal.h"
#include "include/disk.h"
#include "include/fs.h"
#include <stdint.h>
#include "stdio.h"

// Global state
static bool installer_initialized = false;
static install_status_t current_status = INSTALL_SUCCESS;
static install_config_t current_config;
static uint8_t current_state = INSTALL_STATE_INIT;
static char error_message[256] = "";

// Implementation of installer functions
bool installer_init(void) {
    if (installer_initialized) {
        return true;
    }
    
    // Initialize installer state
    installer_initialized = true;
    current_status = INSTALL_SUCCESS;
    current_state = INSTALL_STATE_INIT;
    memset(&current_config, 0, sizeof(current_config));
    memset(error_message, 0, sizeof(error_message));
    
    return true;
}

void installer_get_targets(install_target_t* targets, int* count) {
    (void)targets;  // Mark as intentionally unused
    
    // Get available drives
    disk_drive_t drives[4];  // Support up to 4 drives
    int drive_count = 0;
    disk_get_drives(drives, &drive_count);
    
    // Set count to 0 for now
    *count = 0;
    
    // TODO: Implement target detection
    // This will involve:
    // 1. Checking each drive for valid partitions
    // 2. Checking for free space
    // 3. Populating the targets array
}

install_status_t installer_install(const install_config_t* config) {
    (void)config;  // Unused parameter
    return INSTALL_ERROR_NO_TARGETS;
}

const char* installer_status_message(install_status_t status) {
    switch (status) {
        case INSTALL_SUCCESS:
            return "Installation successful";
        case INSTALL_ERROR_NO_TARGETS:
            return "No installation targets available";
        case INSTALL_ERROR_INVALID_TARGET:
            return "Invalid installation target";
        case INSTALL_ERROR_IO:
            return "I/O error during installation";
        case INSTALL_ERROR_FORMAT:
            return "Error formatting drive";
        case INSTALL_ERROR_PARTITION:
            return "Error partitioning drive";
        case INSTALL_ERROR_FILESYSTEM:
            return "Error creating filesystem";
        case INSTALL_ERROR_BOOTLOADER:
            return "Error installing bootloader";
        default:
            return "Unknown error";
    }
}

bool installer_start(const install_config_t* config) {
    (void)config;  // Unused parameter
    return false;
}

bool installer_partition_disk(void) {
    return false;
}

bool installer_format_partition(void) {
    return false;
}

bool installer_copy_files(void) {
    return false;
}

bool installer_configure_system(void) {
    return false;
}

void installer_cleanup(void) {
    current_state = INSTALL_STATE_INIT;
}

uint8_t installer_get_state(void) {
    return current_state;
}

const char* installer_get_error(void) {
    return error_message;
} 