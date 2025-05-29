#include <stdint.h>
#include <stdbool.h>
#include "include/cpu.h"
#include "include/asm.h"

// CPU information
static cpu_info_t cpu_info;

// CPU features
static uint32_t cpu_features_edx = 0;
static uint32_t cpu_features_ecx = 0;

void cpu_init(void) {
    uint32_t eax, ebx, ecx, edx;
    
    // Get CPU features
    cpuid(CPUID_FEATURES_EDX, &eax, &ebx, &ecx, &edx);
    cpu_features_edx = edx;
    
    cpuid(CPUID_FEATURES_ECX, &eax, &ebx, &ecx, &edx);
    cpu_features_ecx = ecx;
    
    // Get CPU information
    cpu_get_info(&cpu_info);
    
    // Enable CPU features
    cpu_enable_features();
}

void cpu_get_info(cpu_info_t* info) {
    uint32_t eax, ebx, ecx, edx;
    
    // Get CPU vendor ID
    cpuid(0, &eax, &ebx, &ecx, &edx);
    
    info->vendor_id[0] = ebx;
    info->vendor_id[1] = edx;
    info->vendor_id[2] = ecx;
    
    // Get CPU features
    cpuid(1, &eax, &ebx, &ecx, &edx);
    
    info->features = edx;
    info->family = (eax >> 8) & 0xF;
    info->model = (eax >> 4) & 0xF;
    info->stepping = eax & 0xF;
    
    // Get vendor string
    char* vendor = (char*)info->vendor_id;
    info->vendor_string[0] = vendor[0];
    info->vendor_string[1] = vendor[1];
    info->vendor_string[2] = vendor[2];
    info->vendor_string[3] = vendor[3];
    info->vendor_string[4] = vendor[4];
    info->vendor_string[5] = vendor[5];
    info->vendor_string[6] = vendor[6];
    info->vendor_string[7] = vendor[7];
    info->vendor_string[8] = vendor[8];
    info->vendor_string[9] = vendor[9];
    info->vendor_string[10] = vendor[10];
    info->vendor_string[11] = vendor[11];
    info->vendor_string[12] = '\0';
}

uint32_t cpu_get_features(void) {
    return cpu_info.features;
}

// Check if CPU has specific feature
bool cpu_has_feature(uint32_t feature) {
    return (cpu_features_edx & feature) != 0;
}

// Enable CPU features
void cpu_enable_features(void) {
    uint32_t cr4 = READ_CR4();
    
    // Enable PAE if supported
    if (cpu_has_feature(CPUID_FEATURE_PAE)) {
        cr4 |= CPUID_FEATURE_PAE;
    }
    
    // Enable PSE if supported
    if (cpu_has_feature(CPUID_FEATURE_PSE)) {
        cr4 |= CPUID_FEATURE_PSE;
    }
    
    // Enable PGE if supported
    if (cpu_has_feature(CPUID_FEATURE_PGE)) {
        cr4 |= CPUID_FEATURE_PGE;
    }
    
    // Write back control registers
    WRITE_CR4(cr4);
}

// Enable interrupts
void enable_interrupts(void) {
    STI();
}

// Disable interrupts
void disable_interrupts(void) {
    CLI();
}

// Halt CPU
void halt_cpu(void) {
    HLT();
}

// Enable paging
void enable_paging(uint64_t* page_dir) {
    // Load the page directory
    load_page_directory((uint32_t*)page_dir);
    
    // Enable paging
    uint32_t cr0 = READ_CR0();
    cr0 |= 0x80000000;  // Set PG bit
    WRITE_CR0(cr0);
    
    // Flush TLB
    flush_tlb();
}

// Disable paging
void disable_paging(void) {
    uint32_t cr0 = READ_CR0();
    cr0 &= ~0x80000000;  // Clear PG bit
    WRITE_CR0(cr0);
}

// Load page directory
void load_page_directory(uint32_t* page_directory) {
    WRITE_CR3((uint32_t)page_directory);
}

// Flush TLB
void flush_tlb(void) {
    uint32_t cr3 = READ_CR3();
    WRITE_CR3(cr3);
} 