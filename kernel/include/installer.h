#ifndef INSTALLER_H
#define INSTALLER_H

#include <stdint.h>
#include <stdbool.h>

// Default partition sizes (in MB)
#define DEFAULT_ROOT_SIZE 1024
#define DEFAULT_SWAP_SIZE 512

// Maximum number of installation targets
#define MAX_INSTALL_TARGETS 8

// Installation status codes
typedef enum {
    INSTALL_SUCCESS = 0,
    INSTALL_ERROR_NO_TARGETS,
    INSTALL_ERROR_INVALID_TARGET,
    INSTALL_ERROR_IO,
    INSTALL_ERROR_FORMAT,
    INSTALL_ERROR_PARTITION,
    INSTALL_ERROR_FILESYSTEM,
    INSTALL_ERROR_BOOTLOADER
} install_status_t;

// Installation target structure
typedef struct {
    char model[32];
    char drive[16];
    uint32_t size_mb;
    bool is_removable;
} install_target_t;

// Installation configuration
typedef struct {
    char target_drive[16];
    uint32_t root_size_mb;
    uint32_t swap_size_mb;
    bool format_drive;
    char hostname[32];
} install_config_t;

// Function declarations
bool installer_init(void);
void installer_get_targets(install_target_t* targets, int* count);
install_status_t installer_install(const install_config_t* config);
const char* installer_status_message(install_status_t status);

#endif // INSTALLER_H 