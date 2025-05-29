#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "include/stdlib.h"
#include "include/stdio.h"
#include "include/io.h"
#include "include/memory.h"
#include "include/cpu.h"
#include "include/pic.h"
#include "include/terminal.h"
#include "include/kernel.h"
#include "include/keyboard.h"
#include "include/installer.h"
#include "include/multiboot.h"
#include "include/idt.h"
#include "include/splash.h"
#include "drivers/network.h"
#include "include/desktop.h"
#include "include/mouse.h"
#include "include/fs.h"

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
#define VGA_COLOR_LIGHT_GREEN 10

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
extern uint64_t start;
extern uint64_t end;

// Compiler-specific definitions
#if defined(__GNUC__)
    #define USED __attribute__((used))
#else
    #define USED
#endif

// External assembly functions
extern void enable_interrupts(void);
extern void halt_cpu(void);
extern void enable_paging(uint64_t* page_dir);

// Function declarations
static void early_print(const char* str, uint8_t color);
void detect_memory(MultibootInfo* mboot_ptr);
void handle_command(const char* command);
void kernel_init(void);
bool init_paging(void);
void itoa(int value, char* str, int base);
void print_help(void);

// Global system state
uint64_t* page_directory = NULL;
uint64_t* page_tables[1024] = {NULL};
bool pages_used[MAX_PAGES] = {false};

// Terminal state is managed in terminal.c

// Global variables
MultibootInfo* multiboot_info;
uint32_t multiboot_magic;

// Memory tracking
static uint32_t total_memory = 0;
static uint32_t used_memory = 0;
static uint32_t free_memory = 0;

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

// Memory detection function
void detect_memory(MultibootInfo* mboot_ptr) {
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

    total_memory = mboot_ptr->mem_lower + mboot_ptr->mem_upper;
    early_print("Total memory: ", VGA_COLOR_WHITE);
    itoa(total_memory / 1024, buf, 10);
    early_print(buf, VGA_COLOR_WHITE);
    early_print(" MB\n", VGA_COLOR_WHITE);

    // Check for detailed memory map
    if (mboot_ptr->flags & (1 << 6)) {
        struct memory_map_entry* mmap = (struct memory_map_entry*)(uintptr_t)mboot_ptr->mmap_addr;
        uintptr_t end_addr = (uintptr_t)mboot_ptr->mmap_addr + mboot_ptr->mmap_length;

        early_print("\nDetailed Memory Map:\n", VGA_COLOR_WHITE);

        while ((uintptr_t)mmap < end_addr) {
            if (mmap->type == 1) {  // Available memory
                free_memory += mmap->length;
            } else {
                used_memory += mmap->length;
            }

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

            mmap = (struct memory_map_entry*)((uintptr_t)mmap + sizeof(struct memory_map_entry));
        }

        // Print memory summary
        early_print("\nMemory Summary:\n", VGA_COLOR_WHITE);
        early_print("Total Memory: ", VGA_COLOR_WHITE);
        itoa(total_memory / 1024, buf, 10);
        early_print(buf, VGA_COLOR_WHITE);
        early_print(" MB\n", VGA_COLOR_WHITE);

        early_print("Used Memory: ", VGA_COLOR_WHITE);
        itoa(used_memory / (1024 * 1024), buf, 10);
        early_print(buf, VGA_COLOR_WHITE);
        early_print(" MB\n", VGA_COLOR_WHITE);

        early_print("Free Memory: ", VGA_COLOR_WHITE);
        itoa(free_memory / (1024 * 1024), buf, 10);
        early_print(buf, VGA_COLOR_WHITE);
        early_print(" MB\n", VGA_COLOR_WHITE);
    }

    early_print("Memory Detection Complete\n", VGA_COLOR_WHITE);
}

// Initialize kernel subsystems
void kernel_init(void) {
    // Initialize terminal
    terminal_initialize();
    terminal_clear();
    terminal_set_color(VGA_COLOR_LIGHT_GREEN);
    terminal_write_string("Initializing kernel...\n");
    
    // Initialize CPU
    cpu_init();
    
    // Initialize memory
    memory_init();
    
    // Initialize I/O
    io_init();
    
    // Initialize keyboard
    keyboard_init();
    
    // Initialize mouse
    mouse_init();
    
    // Initialize filesystem
    fs_init();
    
    // Initialize installer
    installer_init();
    
    // Initialize desktop
    desktop_init();
    
    // Show splash screen
    splash_show();
    
    terminal_write_string("Kernel initialized successfully!\n");
}

// Main kernel function
void kernel_main(uint32_t magic, MultibootInfo* info) {
    // Check if we were loaded by a multiboot-compliant bootloader
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        early_print("Error: Not loaded by a multiboot-compliant bootloader\n", VGA_COLOR_RED);
        early_print("Magic number: 0x", VGA_COLOR_RED);
        char buf[32];
        itoa(magic, buf, 16);
        early_print(buf, VGA_COLOR_RED);
        early_print("\n", VGA_COLOR_RED);
        return;
    }
    
    // Save multiboot information
    multiboot_info = info;
    multiboot_magic = magic;
    
    // Initialize terminal
    terminal_initialize();
    terminal_write_string("Welcome to ArcOS!\n");
    terminal_write_string("Type 'help' for available commands.\n\n");

    // Initialize keyboard
    keyboard_init();

    // Main loop
    char command[256];
    while (1) {
        terminal_write_string("> ");
        if (keyboard_getline(command, sizeof(command)) > 0) {
            handle_command(command);
        }
    }
}

// Handle user commands
void handle_command(const char* command) {
    if (strcmp(command, "help") == 0) {
        print_help();
    }
    else if (strcmp(command, "install") == 0) {
        install_target_t targets[MAX_INSTALL_TARGETS];
        int target_count = 0;
        install_status_t status;
        install_config_t config;

        // Initialize installer
        if (!installer_init()) {
            terminal_write_string("Failed to initialize installer\n");
            return;
        }

        // Get available installation targets
        installer_get_targets(targets, &target_count);
        if (target_count == 0) {
            terminal_write_string("No installation targets found\n");
            return;
        }

        // Display available targets
        terminal_write_string("Available installation targets:\n");
        for (int i = 0; i < target_count; i++) {
            terminal_write_string(targets[i].model);
            terminal_write_string(" (");
            terminal_write_string(targets[i].drive);
            terminal_write_string(")\n");
        }

        // Get user selection
        terminal_write_string("Select target (0-");
        char num[8];
        sprintf(num, "%d", target_count - 1);
        terminal_write_string(num);
        terminal_write_string("): ");

        char selection_str[8];
        if (keyboard_getline(selection_str, sizeof(selection_str)) <= 0) {
            return;
        }

        int selection = atoi(selection_str);
        if (selection < 0 || selection >= target_count) {
            terminal_write_string("Invalid selection\n");
            return;
        }

        // Configure installation
        strncpy(config.target_drive, targets[selection].drive, sizeof(config.target_drive) - 1);
        config.root_size_mb = DEFAULT_ROOT_SIZE;
        config.swap_size_mb = DEFAULT_SWAP_SIZE;
        config.format_drive = true;
        strncpy(config.hostname, "os", sizeof(config.hostname) - 1);

        // Start installation
        terminal_write_string("Starting installation...\n");
        status = installer_install(&config);
        terminal_write_string(installer_status_message(status));
        terminal_write_string("\n");
    }
    else if (strcmp(command, "clear") == 0) {
        terminal_clear();
    }
    else if (strcmp(command, "exit") == 0) {
        terminal_write_string("Goodbye!\n");
        // TODO: Implement proper shutdown
        while (1) {
            __asm__ __volatile__("hlt");
        }
    }
    else {
        terminal_write_string("Unknown command. Type 'help' for available commands.\n");
    }
}

// Print help information
void print_help(void) {
    terminal_write_string("Available commands:\n");
    terminal_write_string("  help    - Show this help message\n");
    terminal_write_string("  install - Start system installation\n");
    terminal_write_string("  clear   - Clear the screen\n");
    terminal_write_string("  exit    - Exit the system\n");
}

// Initialize terminal is defined in terminal.c

// Initialize paging with error checking
bool init_paging(void) {
    early_print("Initializing paging...\n", VGA_COLOR_WHITE);
    
    // Allocate page directory
    page_directory = (uint64_t*)kmalloc(PAGE_SIZE);
    if (!page_directory) {
        early_print("ERROR: Failed to allocate page directory\n", VGA_COLOR_RED);
        return false;
    }
    memset(page_directory, 0, PAGE_SIZE);

    // Allocate page tables
    for (int i = 0; i < 1024; i++) {
        page_tables[i] = (uint64_t*)kmalloc(PAGE_SIZE);
        if (!page_tables[i]) {
            early_print("ERROR: Failed to allocate page table\n", VGA_COLOR_RED);
            kfree(page_directory);
            return false;
        }
        memset(page_tables[i], 0, PAGE_SIZE);
    }

    // Map first 4MB of memory
    for (int i = 0; i < 1024; i++) {
        page_tables[0][i] = (i * PAGE_SIZE) | 3;  // Present, read/write
    }

    // Map page directory
    page_directory[0] = ((uintptr_t)page_tables[0]) | 3;  // Present, read/write
    page_directory[1023] = ((uintptr_t)page_directory) | 3;  // Present, read/write
    
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
