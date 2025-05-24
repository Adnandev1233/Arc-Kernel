#ifndef INSTALLER_H
#define INSTALLER_H

#include <stdbool.h>
#include <stdint.h>

// Installation status codes
enum install_status {
    INSTALL_STATUS_OK = 0,
    INSTALL_STATUS_ERROR,
    INSTALL_STATUS_IN_PROGRESS,
    INSTALL_STATUS_CANCELLED
};

// Installation target structure
struct install_target {
    char model[64];
    uint32_t size_mb;
    uint8_t drive;
};

// Installation configuration
struct install_config {
    uint8_t target_drive;
    uint32_t root_size_mb;
    uint32_t swap_size_mb;
    bool format_drive;
    char hostname[64];
};

// Default values
#define DEFAULT_ROOT_SIZE 1024  // 1GB
#define DEFAULT_SWAP_SIZE 512   // 512MB

// Function declarations
bool installer_init(void);
int installer_get_targets(struct install_target* targets, int* count);
enum install_status installer_install(const struct install_config* config);
const char* installer_status_message(enum install_status status);

#endif // INSTALLER_H 