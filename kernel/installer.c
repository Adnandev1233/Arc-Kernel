#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include "../include/installer.h"
#include <terminal.h>

// Global state
static bool installer_initialized = false;
static enum install_status current_status = INSTALL_STATUS_OK;

// Helper function to convert integer to string
static void itoa(int value, char* str, int base) {
    char* ptr = str, *ptr1 = str, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789abcdef"[tmp_value - value * base];
    } while (value);

    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}

// Implementation of installer functions
bool installer_init(void) {
    if (installer_initialized) {
        return true;
    }
    
    // Initialize installer state
    installer_initialized = true;
    current_status = INSTALL_STATUS_OK;
    
    return true;
}

int installer_get_targets(struct install_target* targets, int* count) {
    if (!installer_initialized || !targets || !count) {
        return 0;
    }
    
    // For now, return a dummy target
    targets[0].drive = 0x80;  // First hard drive
    strcpy(targets[0].model, "Dummy Drive");
    targets[0].size_mb = 1024;  // 1GB
    
    *count = 1;
    return 1;
}

enum install_status installer_install(const struct install_config* config) {
    if (!installer_initialized || !config) {
        return INSTALL_STATUS_ERROR;
    }
    
    // Simulate installation
    terminal_writestring("Installing to drive ");
    char drive_str[8];
    itoa(config->target_drive, drive_str, 16);
    terminal_writestring(drive_str);
    terminal_writestring("\n");
    
    return INSTALL_STATUS_OK;
}

const char* installer_status_message(enum install_status status) {
    switch (status) {
        case INSTALL_STATUS_OK:
            return "Installation completed successfully";
        case INSTALL_STATUS_ERROR:
            return "Installation failed";
        case INSTALL_STATUS_IN_PROGRESS:
            return "Installation in progress";
        case INSTALL_STATUS_CANCELLED:
            return "Installation cancelled";
        default:
            return "Unknown status";
    }
} 