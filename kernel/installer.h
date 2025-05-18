#ifndef INSTALLER_H
#define INSTALLER_H

#include <stdint.h>
#include <stdbool.h>

// Default partition sizes in MB
#define DEFAULT_ROOT_SIZE 1024     // 1GB default root size
#define DEFAULT_SWAP_SIZE 512      // 512MB swap
#define MIN_ROOT_SIZE 1024         // Minimum 1GB for root
#define MAX_ROOT_SIZE 8192         // Maximum 8GB for root

// Installation target structure
struct install_target {
    uint8_t drive;          // Drive number
    uint32_t size_mb;       // Drive size in MB
    char model[64];         // Drive model name
};

// Installation configuration
struct install_config {
    uint8_t target_drive;   // Target drive number
    uint32_t root_size_mb;  // Root partition size in MB
    uint32_t swap_size_mb;  // Swap partition size in MB
    bool format_drive;      // Whether to format drive
    char hostname[64];      // System hostname
};

// Installation status codes
enum install_status {
    INSTALL_SUCCESS = 0,
    INSTALL_ERROR_INIT,
    INSTALL_ERROR_DISK,
    INSTALL_ERROR_PARTITION,
    INSTALL_ERROR_FORMAT,
    INSTALL_ERROR_COPY,
    INSTALL_ERROR_BOOTLOADER
};

// Function declarations
bool installer_init(void);
void installer_get_targets(struct install_target* targets, int* count);
enum install_status installer_install(const struct install_config* config);
const char* installer_status_message(enum install_status status);

#endif /* INSTALLER_H */ 