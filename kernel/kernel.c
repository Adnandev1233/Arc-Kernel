#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "string.h"
#include "stdlib.h"
#include "io.h"
#include "memory.h"
#include "cpu.h"
#include "pic.h"
#include "terminal.h"
#include "kernel.h"
#include "keyboard.h"
#include "installer.h"
#include "../kernel/include/multiboot.h"
#include <idt.h>

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
void detect_memory(struct multiboot_info* mboot_ptr);
void handle_command(void);
void keyboard_handler(void);
void kernel_init(void);
bool init_paging(void);
void* kmalloc(size_t size);
void kfree(void* ptr);
void itoa(int value, char* str, int base);

// Global system state
uint32_t* page_directory = NULL;
uint32_t* page_tables[1024] = {NULL};
bool pages_used[MAX_PAGES] = {false};

// Terminal state is managed in terminal.c

// Command buffer and history
static char command_buffer[KEYBOARD_BUFFER_SIZE];
static size_t command_pos = 0;
static char command_history[COMMAND_HISTORY_SIZE][KEYBOARD_BUFFER_SIZE];
static int history_count = 0;
static int history_position = -1;

// Early debug output function with color support
static void early_print(const char* str, uint8_t color) {
    volatile uint16_t* vga = (uint16_t*)0xB8000;
    static size_t pos = 0;
    
    if (pos == 0) {  // Clear screen on first use
        for(int i = 0; i < 80 * 25; i++) {
            vga[i] = (uint16_t)' ' | (uint16_t)VGA_COLOR_BLACK << 8;
        }
    }
    
    for(size_t i = 0; str[i] != '\0'; i++) {
        if(str[i] == '\n') {
            pos = (pos + 80) - (pos % 80);
        } else {
            vga[pos++] = (uint16_t)str[i] | (uint16_t)color << 8;
        }
        if(pos >= 80 * 25) {  // Screen full, reset to top
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
void detect_memory(struct multiboot_info* mboot_ptr) {
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

            mmap = (struct memory_map_entry*)((uint32_t)mmap + mmap->size + 4);
        }
    }

    early_print("Memory Detection Complete\n", VGA_COLOR_WHITE);
}

// Entry point
void kernel_main(uint32_t magic, struct multiboot_info* mboot_ptr) {
    // Early debug output
    early_print("Kernel starting...\n", VGA_COLOR_WHITE);
    
    // Check multiboot magic
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        early_print("ERROR: Invalid multiboot magic!\n", VGA_COLOR_RED);
        return;
    }
    
    // Initialize memory BEFORE paging
    detect_memory(mboot_ptr);
    
    // Wait a moment to ensure messages are visible
    for(volatile int i = 0; i < 100000; i++);
    
    // Initialize paging with memory info
    if (!init_paging()) {
        early_print("ERROR: Failed to initialize paging!\n", VGA_COLOR_RED);
        return;
    }
    
    // Wait a moment to ensure messages are visible
    for(volatile int i = 0; i < 100000; i++);
    
    // Initialize terminal first (after paging is set up)
    early_print("Initializing terminal...\n", VGA_COLOR_WHITE);
    init_terminal();
    
    // Initialize PIC before IDT
    early_print("Initializing PIC...\n", VGA_COLOR_WHITE);
    init_pic();
    
    // Initialize IDT after PIC
    early_print("Initializing IDT...\n", VGA_COLOR_WHITE);
    init_idt();
    
    // Initialize keyboard buffer
    command_buffer[0] = '\0';
    command_pos = 0;
    history_count = 0;
    history_position = -1;
    
    // Clear screen and display welcome message
    init_terminal();  // Clear screen again
    terminal_writestring("ArcOS Shell v0.1\n");
    terminal_writestring("Type 'help' for available commands\n");
    terminal_writestring("[user@arcos:~$] ");
    
    // Enable interrupts only after all initialization is complete
    early_print("Enabling interrupts...\n", VGA_COLOR_WHITE);
    enable_interrupts();
    
    early_print("System initialization complete!\n", VGA_COLOR_WHITE);
    
    // Main kernel loop - use halt instead of busy waiting
    while(1) {
        halt_cpu();  // Wait for next interrupt
    }
}

// Command handler
void handle_command(void) {
    if (command_buffer[0] == '\0') {
        terminal_writestring("[user@arcos:~$] ");
        return;
    }
    
    // Save command to history
    save_to_history(command_buffer);
    
    if (strcmp(command_buffer, "help") == 0) {
        terminal_writestring("Available commands:\n");
        terminal_writestring("  help     - Show this help message\n");
        terminal_writestring("  clear    - Clear the screen\n");
        terminal_writestring("  about    - Show system information\n");
        terminal_writestring("  memory   - Show memory usage\n");
        terminal_writestring("  history  - Show command history\n");
        terminal_writestring("  uptime   - Show system uptime\n");
        terminal_writestring("  install  - Install system to disk\n");
        terminal_writestring("  reboot   - Reboot the system\n");
        terminal_writestring("  shutdown - Halt the system\n");
    }
    else if (strcmp(command_buffer, "install") == 0) {
        struct install_target targets[8];
        int target_count = 8;
        struct install_config config;
        enum install_status status;
        char input[32];
        int selection;
        
        // Initialize installer
        if (!installer_init()) {
            terminal_writestring("Failed to initialize installer\n");
            return;
        }
        
        // Get available targets
        installer_get_targets(targets, &target_count);
        if (target_count == 0) {
            terminal_writestring("No installation targets found\n");
            return;
        }
        
        // Show available targets
        terminal_writestring("Available installation targets:\n\n");
        for (int i = 0; i < target_count; i++) {
            char num[8];
            itoa(i + 1, num, 10);
            terminal_writestring(num);
            terminal_writestring(". ");
            terminal_writestring(targets[i].model);
            terminal_writestring(" (");
            itoa(targets[i].size_mb, num, 10);
            terminal_writestring(num);
            terminal_writestring(" MB)\n");
        }
        
        // Get user selection
        terminal_writestring("\nSelect target disk (1-");
        char num[8];
        itoa(target_count, num, 10);
        terminal_writestring(num);
        terminal_writestring("): ");
        
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
                terminal_putchar(c);
            }
        }
        terminal_writestring("\n");
        
        selection = atoi(input) - 1;
        if (selection < 0 || selection >= target_count) {
            terminal_writestring("Invalid selection\n");
            return;
        }
        
        // Configure installation
        config.target_drive = targets[selection].drive;
        config.root_size_mb = DEFAULT_ROOT_SIZE;
        config.swap_size_mb = DEFAULT_SWAP_SIZE;
        config.format_drive = true;
        strcpy(config.hostname, "arcos");
        
        // Confirm installation
        terminal_writestring("\nWARNING: This will erase all data on ");
        terminal_writestring(targets[selection].model);
        terminal_writestring("\nType 'yes' to continue: ");
        
        command_pos = 0;
        while (1) {
            char c = keyboard_getchar();
            if (c == '\n') {
                input[command_pos] = '\0';
                break;
            }
            if (command_pos < sizeof(input) - 1) {
                input[command_pos++] = c;
                terminal_putchar(c);
            }
        }
        terminal_writestring("\n");
        
        if (strcmp(input, "yes") != 0) {
            terminal_writestring("Installation cancelled\n");
            return;
        }
        
        // Perform installation
        terminal_writestring("\nInstalling system...\n");
        status = installer_install(&config);
        
        terminal_writestring("\n");
        terminal_writestring(installer_status_message(status));
        terminal_writestring("\n");
    }
    else if (strcmp(command_buffer, "clear") == 0) {
        init_terminal();
    }
    else if (strcmp(command_buffer, "about") == 0) {
        terminal_writestring("ArcOS - A simple operating system\n");
        terminal_writestring("Version: 0.1\n");
        terminal_writestring("Features:\n");
        terminal_writestring("  - Protected Mode\n");
        terminal_writestring("  - Paging enabled\n");
        terminal_writestring("  - Interrupt handling\n");
        terminal_writestring("  - Basic shell interface\n");
        terminal_writestring("  - Memory management\n");
    }
    else if (strcmp(command_buffer, "memory") == 0) {
        size_t used_pages = 0;
        for (size_t i = 0; i < MAX_PAGES; i++) {
            if (pages_used[i]) used_pages++;
        }
        terminal_writestring("Memory usage:\n");
        terminal_writestring("  Used pages: ");
        char num[16];
        itoa(used_pages, num, 10);
        terminal_writestring(num);
        terminal_writestring("/");
        itoa(MAX_PAGES, num, 10);
        terminal_writestring(num);
        terminal_writestring("\n");
        terminal_writestring("  Page size: 4KB\n");
        terminal_writestring("  Total memory: 4MB\n");
    }
    else if (strcmp(command_buffer, "history") == 0) {
        if (history_count == 0) {
            terminal_writestring("No commands in history\n");
        } else {
            for (int i = 0; i < history_count; i++) {
                char num[8];
                itoa(i + 1, num, 10);
                terminal_writestring(" ");
                terminal_writestring(num);
                terminal_writestring("  ");
                terminal_writestring(command_history[i]);
                terminal_writestring("\n");
            }
        }
    }
    else if (strcmp(command_buffer, "uptime") == 0) {
        // Note: This is a placeholder since we don't have a real timer yet
        terminal_writestring("System uptime: Not implemented yet\n");
    }
    else if (strcmp(command_buffer, "reboot") == 0) {
        terminal_writestring("Rebooting system...\n");
        // Send reset CPU command
        port_out_byte(0x64, 0xFE);
    }
    else if (strcmp(command_buffer, "shutdown") == 0) {
        terminal_writestring("System halted.\n");
        // Disable interrupts and halt
        enable_interrupts(); // First enable them to ensure clean shutdown
        halt_cpu();         // Then halt the CPU
    }
    else {
        terminal_writestring("Unknown command: ");
        terminal_writestring(command_buffer);
        terminal_writestring("\nType 'help' for available commands\n");
    }
    
    terminal_writestring("[user@arcos:~$] ");
}

// Initialize kernel subsystems
void kernel_init(void) {
    init_paging();
    init_terminal();
    
    terminal_writestring("Initializing PIC...\n");
    init_pic();
    
    terminal_writestring("Initializing IDT...\n");
    init_idt();
    
    terminal_writestring("Initialization complete.\n");
}

// Initialize terminal is defined in terminal.c

// Initialize paging with error checking
bool init_paging(void) {
    early_print("Initializing Paging...\n", VGA_COLOR_WHITE);
    
    // Calculate addresses for page directory and first page table
    // Use physical addresses above kernel but below 1MB to ensure they're accessible
    uint32_t* page_directory = (uint32_t*)0x9C000;  // At 624KB
    uint32_t* first_page_table = (uint32_t*)0x9D000; // At 628KB
    
    early_print("Clearing page directory...\n", VGA_COLOR_WHITE);
    
    // Clear page directory first
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000002;  // Supervisor, write-enabled, not present
    }
    
    early_print("Setting up initial page table...\n", VGA_COLOR_WHITE);
    
    // Identity map first 4MB (includes VGA, BIOS, and kernel space)
    for (int i = 0; i < 1024; i++) {
        // Attributes: supervisor level, read/write, present
        first_page_table[i] = (i * 0x1000) | 3;
    }
    
    // Map VGA memory (0xB8000) to make sure it's accessible
    int vga_offset = 0xB8000 >> 12;  // Convert to page number
    first_page_table[vga_offset] = 0xB8000 | 3;  // Present, read/write
    
    // Add the page table to the page directory
    page_directory[0] = ((uint32_t)first_page_table) | 3;  // Supervisor, present, write-enabled
    
    // Map the kernel pages (assuming kernel is loaded at 1MB)
    uint32_t* kernel_page_table = (uint32_t*)0x9E000;  // At 632KB
    for (int i = 0; i < 1024; i++) {
        kernel_page_table[i] = (0x100000 + (i * 0x1000)) | 3;  // Map 1MB onwards
    }
    page_directory[1] = ((uint32_t)kernel_page_table) | 3;
    
    // Also map the page directory to itself (recursive mapping)
    page_directory[1023] = ((uint32_t)page_directory) | 3;
    
    early_print("Enabling paging...\n", VGA_COLOR_WHITE);
    
    // Load page directory and enable paging
    enable_paging((uint32_t*)page_directory);
    
    early_print("Paging enabled successfully\n", VGA_COLOR_WHITE);
    return true;
}

// Memory allocation
void* kmalloc(size_t size) {
    size_t pages_needed = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    size_t consecutive_pages = 0;
    size_t start_page = 0;
    
    for(size_t i = 0; i < MAX_PAGES; i++) {
        if(!pages_used[i]) {
            if(consecutive_pages == 0) start_page = i;
            consecutive_pages++;
            if(consecutive_pages >= pages_needed) {
                for(size_t j = start_page; j < start_page + pages_needed; j++) {
                    pages_used[j] = true;
                }
                return (void*)(start_page * PAGE_SIZE);
            }
        } else {
            consecutive_pages = 0;
        }
    }
    return NULL;
}

void kfree(void* ptr) {
    size_t page = (size_t)ptr / PAGE_SIZE;
    while(page < MAX_PAGES && pages_used[page]) {
        pages_used[page] = false;
        page++;
    }
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

// Keyboard handler
void keyboard_handler(void) {
    // Check if there's actually data to read
    if ((port_in_byte(KEYBOARD_STATUS_PORT) & 0x01) == 0) {
        // No data available, spurious interrupt
        port_out_byte(0x20, 0x20);  // Send EOI
        return;
    }

    uint8_t scancode = keyboard_get_scancode();
    
    // Handle special keys first
    if (keyboard_is_special_key(scancode)) {
        switch (scancode) {
            case KEY_UP:
                if (history_position < history_count - 1) {
                    // Clear current line
                    while (command_pos > 0) {
                        terminal_putchar('\b');
                        command_pos--;
                    }
                    
                    // Copy history entry
                    history_position++;
                    strcpy(command_buffer, command_history[history_position]);
                    command_pos = strlen(command_buffer);
                    
                    // Display command
                    terminal_writestring(command_buffer);
                }
                break;
                
            case KEY_DOWN:
                // Clear current line
                while (command_pos > 0) {
                    terminal_putchar('\b');
                    command_pos--;
                }
                
                if (history_position > 0) {
                    // Copy previous history entry
                    history_position--;
                    strcpy(command_buffer, command_history[history_position]);
                } else {
                    // Clear command on last down press
                    history_position = -1;
                    command_buffer[0] = '\0';
                }
                
                command_pos = strlen(command_buffer);
                terminal_writestring(command_buffer);
                break;
                
            case KEY_LEFT:
                if (command_pos > 0) {
                    command_pos--;
                    terminal_move_cursor_left();
                }
                break;
                
            case KEY_RIGHT:
                if (command_pos < strlen(command_buffer)) {
                    command_pos++;
                    terminal_move_cursor_right();
                }
                break;
                
            case KEY_HOME:
                while (command_pos > 0) {
                    command_pos--;
                    terminal_move_cursor_left();
                }
                break;
                
            case KEY_END:
                while (command_pos < strlen(command_buffer)) {
                    command_pos++;
                    terminal_move_cursor_right();
                }
                break;
                
            case KEY_DELETE:
                if (command_pos < strlen(command_buffer)) {
                    // Shift characters left
                    memmove(&command_buffer[command_pos],
                           &command_buffer[command_pos + 1],
                           strlen(command_buffer) - command_pos);
                    
                    // Update display
                    terminal_delete_char();
                }
                break;
        }
    }
    else {
        char c = keyboard_getchar();
        
        if (c != 0) {  // Valid character
            if (c == '\b') {  // Backspace
                if (command_pos > 0) {
                    // Move characters left
                    memmove(&command_buffer[command_pos - 1],
                           &command_buffer[command_pos],
                           strlen(command_buffer) - command_pos + 1);
                    command_pos--;
                    terminal_putchar('\b');
                }
            }
            else if (c == '\n') {  // Enter
                terminal_putchar('\n');
                command_buffer[command_pos] = '\0';
                
                // Process command if not empty
                if (command_pos > 0) {
                    handle_command();
                } else {
                    terminal_writestring("[user@arcos:~$] ");
                }
                
                // Reset command buffer
                command_pos = 0;
                command_buffer[0] = '\0';
                history_position = -1;
            }
            else if (command_pos < KEYBOARD_BUFFER_SIZE - 1) {  // Regular character
                // Shift characters right to make room
                memmove(&command_buffer[command_pos + 1],
                       &command_buffer[command_pos],
                       strlen(command_buffer) - command_pos);
                       
                command_buffer[command_pos++] = c;
                terminal_putchar(c);
            }
        }
    }
    
    // Send End of Interrupt (EOI)
    port_out_byte(0x20, 0x20);
}
