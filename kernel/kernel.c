#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <memory.h>
#include <cpu.h>
#include <pic.h>
#include "include/terminal.h"
#include "include/kernel.h"
#include "include/keyboard.h"
#include "include/installer.h"
#include "include/multiboot.h"
#include <idt.h>
#include "include/splash.h"
#include "drivers/network.h"
#include "include/desktop.h"
#include "include/mouse.h"

// Command buffer
char command_buffer[KEYBOARD_BUFFER_SIZE] = {0};
size_t command_pos = 0;

// Constants
#define COMMAND_HISTORY_SIZE 10
#define PAGE_SIZE 4096
#define PAGES_PER_TABLE 1024
#define TABLES_PER_DIRECTORY 1024
#define MAX_MEMORY_MB 4096  // Support up to 4GB
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_BUFFER_SIZE 256

// VGA text mode color constants
#define VGA_COLOR_BLACK 0
#define VGA_COLOR_LIGHT_GREY 7
#define VGA_COLOR_WHITE 15
#define VGA_COLOR_RED 4

// Memory size constants
#define KB (1024)
#define MB (1024 * KB)
#define GB (1024 * MB)

// Multiboot constants
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002
#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
#define MULTIBOOT_FLAG_PAGE_ALIGN (1 << 0)
#define MULTIBOOT_FLAG_MEMORY_INFO (1 << 1)
#define MULTIBOOT_FLAGS (MULTIBOOT_FLAG_PAGE_ALIGN | MULTIBOOT_FLAG_MEMORY_INFO)
#define MULTIBOOT_CHECKSUM -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_FLAGS)

// Kernel start and end addresses (defined by linker)
extern uint32_t start;
extern uint32_t end;

// Compiler-specific definitions
#if defined(__GNUC__)
    #define USED __attribute__((used))
#else
    #define USED
#endif

// External assembly functions
extern void enable_interrupts(void);
extern void halt_cpu(void);
extern void enable_paging(uint32_t* page_dir);

// Function declarations
static void early_print(const char* str, uint8_t color);
static void save_to_history(const char* cmd);
void detect_memory(MultibootInfo* mboot_ptr);
void handle_command(void);
void kernel_init(void);
bool init_paging(void);
void itoa(int value, char* str, int base);

// Global system state
uint32_t* page_directory = NULL;
uint32_t* page_tables[1024] = {NULL};
bool pages_used[MAX_PAGES] = {false};

// Terminal state is managed in terminal.c

// Command buffer and history
static char command_history[COMMAND_HISTORY_SIZE][KEYBOARD_BUFFER_SIZE];
static int history_count = 0;

// Global variables
MultibootInfo* multiboot_info;
uint32_t multiboot_magic;

// Early debug output function with color support
static void early_print(const char* str, uint8_t color) {
    volatile uint16_t* vga = (uint16_t*)VGA_MEMORY;
    static size_t pos = 0;
    
    for(size_t i = 0; str[i] != '\0'; i++) {
        if(str[i] == '\n') {
            pos = (pos + VGA_WIDTH) - (pos % VGA_WIDTH);
        } else {
            vga[pos++] = (uint16_t)str[i] | (uint16_t)color << 8;
        }
        if(pos >= VGA_WIDTH * VGA_HEIGHT) {
            pos = 0;
        }
    }
}

// Save command to history
static void save_to_history(const char* cmd) {
    if (strlen(cmd) == 0) return;
    
    // Shift history if full
    if (history_count == COMMAND_HISTORY_SIZE) {
        for (int i = 0; i < COMMAND_HISTORY_SIZE - 1; i++) {
            strcpy(command_history[i], command_history[i + 1]);
        }
        history_count--;
    }
    
    // Add new command
    strcpy(command_history[history_count], cmd);
    history_count++;
}

// Memory detection function
void detect_memory(MultibootInfo* mboot_ptr) {
    uint32_t total_mem = 0;
    char buf[32];

    early_print("Memory Detection Started...\n", VGA_COLOR_WHITE);

    // Check if memory info is available
    if (!(mboot_ptr->flags & (1 << 0))) {
        early_print("ERROR: No basic memory information available!\n", VGA_COLOR_RED);
        return;
    }

    // Print basic memory info
    early_print("Basic Memory Info:\n", VGA_COLOR_WHITE);
    early_print("Lower memory: ", VGA_COLOR_WHITE);
    itoa(mboot_ptr->mem_lower, buf, 10);
    early_print(buf, VGA_COLOR_WHITE);
    early_print(" KB\n", VGA_COLOR_WHITE);

    early_print("Upper memory: ", VGA_COLOR_WHITE);
    itoa(mboot_ptr->mem_upper, buf, 10);
    early_print(buf, VGA_COLOR_WHITE);
    early_print(" KB\n", VGA_COLOR_WHITE);

    total_mem = mboot_ptr->mem_lower + mboot_ptr->mem_upper;
    early_print("Total memory: ", VGA_COLOR_WHITE);
    itoa(total_mem / 1024, buf, 10);
    early_print(buf, VGA_COLOR_WHITE);
    early_print(" MB\n", VGA_COLOR_WHITE);

    // Check for detailed memory map
    if (mboot_ptr->flags & (1 << 6)) {
        struct memory_map_entry* mmap = (struct memory_map_entry*)mboot_ptr->mmap_addr;
        uint32_t end_addr = mboot_ptr->mmap_addr + mboot_ptr->mmap_length;

        early_print("\nDetailed Memory Map:\n", VGA_COLOR_WHITE);

        while ((uint32_t)mmap < end_addr) {
            early_print("Region: Base=0x", VGA_COLOR_WHITE);
            // Print upper 32 bits of base address
            itoa((uint32_t)(mmap->base_addr >> 32), buf, 16);
            early_print(buf, VGA_COLOR_WHITE);
            // Print lower 32 bits of base address
            itoa((uint32_t)(mmap->base_addr & 0xFFFFFFFF), buf, 16);
            early_print(buf, VGA_COLOR_WHITE);
            
            early_print(" Length=0x", VGA_COLOR_WHITE);
            // Print upper 32 bits of length
            itoa((uint32_t)(mmap->length >> 32), buf, 16);
            early_print(buf, VGA_COLOR_WHITE);
            // Print lower 32 bits of length
            itoa((uint32_t)(mmap->length & 0xFFFFFFFF), buf, 16);
            early_print(buf, VGA_COLOR_WHITE);
            
            early_print(" Type=", VGA_COLOR_WHITE);
            itoa(mmap->type, buf, 10);
            early_print(buf, VGA_COLOR_WHITE);
            early_print("\n", VGA_COLOR_WHITE);

            mmap = (struct memory_map_entry*)((uint32_t)mmap + sizeof(struct memory_map_entry));
        }
    }

    early_print("Memory Detection Complete\n", VGA_COLOR_WHITE);
}

// Initialize kernel subsystems
void kernel_init(void) {
    // Initialize terminal
    terminal_initialize();
    terminal_write_string("Initializing terminal...\n");
    
    // Initialize CPU
    cpu_init();
    terminal_write_string("Initializing CPU...\n");
    
    // Initialize network
    if (network_init()) {
        terminal_write_string("Network initialized successfully\n");
    } else {
        terminal_write_string("Failed to initialize network\n");
    }
    
    // Initialize desktop environment
    desktop_init();
    terminal_write_string("Desktop environment initialized\n");
}

// Main kernel function
void kernel_main(uint32_t magic, MultibootInfo* info) {
    // Check if we were loaded by a multiboot-compliant bootloader
    if (magic != 0x2BADB002) {
        terminal_write_string("Error: Not loaded by a multiboot-compliant bootloader\n");
        return;
    }
    
    // Save multiboot information
    multiboot_info = info;
    multiboot_magic = magic;
    
    // Print system information
    terminal_write_string("ArcOS Kernel v1.0\n");
    terminal_write_string("Memory: ");
    terminal_write_dec(info->mem_upper);
    terminal_write_string(" KB available\n");
    
    // Initialize kernel subsystems
    kernel_init();
    
    // Start desktop environment
    desktop_main();
}

// Command handler
void handle_command(void) {
    if (command_buffer[0] == '\0') {
        terminal_write_string("[user@arcos:~$] ");
        return;
    }
    
    // Save command to history
    save_to_history(command_buffer);
    
    if (strcmp(command_buffer, "help") == 0) {
        terminal_write_string("Available commands:\n");
        terminal_write_string("  help     - Show this help message\n");
        terminal_write_string("  clear    - Clear the screen\n");
        terminal_write_string("  about    - Show system information\n");
        terminal_write_string("  memory   - Show memory usage\n");
        terminal_write_string("  history  - Show command history\n");
        terminal_write_string("  uptime   - Show system uptime\n");
        terminal_write_string("  install  - Install system to disk\n");
        terminal_write_string("  reboot   - Reboot the system\n");
        terminal_write_string("  shutdown - Halt the system\n");
    }
    else if (strcmp(command_buffer, "install") == 0) {
        install_target_t targets[MAX_INSTALL_TARGETS];
        int target_count = MAX_INSTALL_TARGETS;
        install_config_t config;
        install_status_t status;
        char input[32];
        int selection;
        
        // Initialize installer
        if (!installer_init()) {
            terminal_write_string("Failed to initialize installer\n");
            return;
        }
        
        // Get available targets
        installer_get_targets(targets, &target_count);
        if (target_count == 0) {
            terminal_write_string("No installation targets found\n");
            return;
        }
        
        // Show available targets
        terminal_write_string("Available installation targets:\n\n");
        for (int i = 0; i < target_count; i++) {
            char num[8];
            itoa(i + 1, num, 10);
            terminal_write_string(num);
            terminal_write_string(". ");
            terminal_write_string(targets[i].model);
            terminal_write_string(" (");
            itoa(targets[i].size_mb, num, 10);
            terminal_write_string(num);
            terminal_write_string(" MB)\n");
        }
        
        // Get user selection
        terminal_write_string("\nSelect target disk (1-");
        char num[8];
        itoa(target_count, num, 10);
        terminal_write_string(num);
        terminal_write_string("): ");
        
        // Wait for input
        command_pos = 0;
        while (1) {
            char c = keyboard_getchar();
            if (c == '\n') {
                input[command_pos] = '\0';
                break;
            }
            if (c >= '0' && c <= '9' && command_pos < sizeof(input) - 1) {
                input[command_pos++] = c;
                terminal_put_char(c);
            }
        }
        terminal_write_string("\n");
        
        selection = atoi(input) - 1;
        if (selection < 0 || selection >= target_count) {
            terminal_write_string("Invalid selection\n");
            return;
        }
        
        // Configure installation
        strncpy(config.target_drive, targets[selection].drive, sizeof(config.target_drive) - 1);
        config.root_size_mb = DEFAULT_ROOT_SIZE;
        config.swap_size_mb = DEFAULT_SWAP_SIZE;
        config.format_drive = true;
        strncpy(config.hostname, "arcos", sizeof(config.hostname) - 1);
        
        // Confirm installation
        terminal_write_string("\nWARNING: This will erase all data on ");
        terminal_write_string(targets[selection].model);
        terminal_write_string("\nType 'yes' to continue: ");
        
        command_pos = 0;
        while (1) {
            char c = keyboard_getchar();
            if (c == '\n') {
                input[command_pos] = '\0';
                break;
            }
            if (command_pos < sizeof(input) - 1) {
                input[command_pos++] = c;
                terminal_put_char(c);
            }
        }
        terminal_write_string("\n");
        
        if (strcmp(input, "yes") != 0) {
            terminal_write_string("Installation cancelled\n");
            return;
        }
        
        // Perform installation
        terminal_write_string("\nInstalling system...\n");
        status = installer_install(&config);
        
        terminal_write_string("\n");
        terminal_write_string(installer_status_message(status));
        terminal_write_string("\n");
    }
    else if (strcmp(command_buffer, "clear") == 0) {
        terminal_initialize();
    }
    else if (strcmp(command_buffer, "about") == 0) {
        terminal_write_string("ArcOS - A simple operating system\n");
        terminal_write_string("Version: 0.1\n");
        terminal_write_string("Features:\n");
        terminal_write_string("  - Protected Mode\n");
        terminal_write_string("  - Paging enabled\n");
        terminal_write_string("  - Interrupt handling\n");
        terminal_write_string("  - Basic shell interface\n");
        terminal_write_string("  - Memory management\n");
    }
    else if (strcmp(command_buffer, "memory") == 0) {
        size_t used_pages = 0;
        for (size_t i = 0; i < MAX_PAGES; i++) {
            if (pages_used[i]) used_pages++;
        }
        terminal_write_string("Memory usage:\n");
        terminal_write_string("  Used pages: ");
        char num[16];
        itoa(used_pages, num, 10);
        terminal_write_string(num);
        terminal_write_string("/");
        itoa(MAX_PAGES, num, 10);
        terminal_write_string(num);
        terminal_write_string("\n");
        terminal_write_string("  Page size: 4KB\n");
        terminal_write_string("  Total memory: 4MB\n");
    }
    else if (strcmp(command_buffer, "history") == 0) {
        if (history_count == 0) {
            terminal_write_string("No commands in history\n");
        } else {
            for (int i = 0; i < history_count; i++) {
                char num[8];
                itoa(i + 1, num, 10);
                terminal_write_string(" ");
                terminal_write_string(num);
                terminal_write_string("  ");
                terminal_write_string(command_history[i]);
                terminal_write_string("\n");
            }
        }
    }
    else if (strcmp(command_buffer, "uptime") == 0) {
        // Note: This is a placeholder since we don't have a real timer yet
        terminal_write_string("System uptime: Not implemented yet\n");
    }
    else if (strcmp(command_buffer, "reboot") == 0) {
        terminal_write_string("Rebooting system...\n");
        // Send reset CPU command
        port_out_byte(0x64, 0xFE);
    }
    else if (strcmp(command_buffer, "shutdown") == 0) {
        terminal_write_string("System halted.\n");
        // Disable interrupts and halt
        enable_interrupts(); // First enable them to ensure clean shutdown
        halt_cpu();         // Then halt the CPU
    }
    else {
        terminal_write_string("Unknown command: ");
        terminal_write_string(command_buffer);
        terminal_write_string("\nType 'help' for available commands\n");
    }
    
    terminal_write_string("[user@arcos:~$] ");
}

// Initialize terminal is defined in terminal.c

// Initialize paging with error checking
bool init_paging(void) {
    early_print("Initializing paging...\n", VGA_COLOR_WHITE);
    
    // Allocate page directory and first page table from kernel heap
    page_directory = (uint32_t*)kmalloc(PAGE_SIZE);
    if (!page_directory) {
        early_print("ERROR: Failed to allocate page directory\n", VGA_COLOR_RED);
        return false;
    }
    
    page_tables[0] = (uint32_t*)kmalloc(PAGE_SIZE);
    if (!page_tables[0]) {
        early_print("ERROR: Failed to allocate first page table\n", VGA_COLOR_RED);
        kfree(page_directory);
        return false;
    }
    
    // Clear page directory
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000002;  // Supervisor, write-enabled, not present
    }
    
    // Clear first page table
    for (int i = 0; i < 1024; i++) {
        page_tables[0][i] = 0x00000002;  // Supervisor, write-enabled, not present
    }
    
    // Identity map first 1MB (kernel space)
    for (int i = 0; i < 256; i++) {
        page_tables[0][i] = (i * 0x1000) | 3;  // Present, read/write
    }
    
    // Map VGA memory (0xB8000)
    int vga_offset = 0xB8000 >> 12;
    page_tables[0][vga_offset] = 0xB8000 | 3;  // Present, read/write
    
    // Add page table to directory
    page_directory[0] = ((uint32_t)page_tables[0]) | 3;  // Present, read/write
    
    // Map page directory to itself
    page_directory[1023] = ((uint32_t)page_directory) | 3;  // Present, read/write
    
    // Enable paging
    enable_paging(page_directory);
    
    early_print("Paging enabled\n", VGA_COLOR_WHITE);
    return true;
}

// Convert integer to string
void itoa(int value, char* str, int base) {
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
