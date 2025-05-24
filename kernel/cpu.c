#include "include/cpu.h"

// CPU information
static cpu_info_t cpu_info;

void cpu_init(void) {
    // Get CPU information
    cpu_get_info(&cpu_info);
    
    // Enable CPU features
    cpu_enable_features();
}

void cpu_get_info(cpu_info_t* info) {
    uint32_t eax, ebx, ecx, edx;
    
    #ifdef __GNUC__
    // Get CPU vendor ID
    __asm__ __volatile__("cpuid"
                        : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
                        : "a" (0)
                        : "memory");
    
    info->vendor_id[0] = ebx;
    info->vendor_id[1] = edx;
    info->vendor_id[2] = ecx;
    
    // Get CPU features
    __asm__ __volatile__("cpuid"
                        : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
                        : "a" (1)
                        : "memory");
    
    info->features = edx;
    info->family = (eax >> 8) & 0xF;
    info->model = (eax >> 4) & 0xF;
    info->stepping = eax & 0xF;
    #endif
    
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

void cpu_enable_features(void) {
    uint32_t cr0, cr4;
    
    #ifdef __GNUC__
    // Read CR0
    __asm__ __volatile__("mov %%cr0, %0" : "=r" (cr0) : : "memory");
    
    // Enable FPU
    cr0 &= ~(1 << 2);  // Clear EM
    cr0 |= (1 << 1);   // Set MP
    
    // Write CR0
    __asm__ __volatile__("mov %0, %%cr0" : : "r" (cr0) : "memory");
    
    // Read CR4
    __asm__ __volatile__("mov %%cr4, %0" : "=r" (cr4) : : "memory");
    
    // Enable SSE if available
    if (cpu_info.features & CPU_FEATURE_SSE) {
        cr4 |= (1 << 9);  // Set OSFXSR
        cr4 |= (1 << 10); // Set OSXMMEXCPT
    }
    
    // Write CR4
    __asm__ __volatile__("mov %0, %%cr4" : : "r" (cr4) : "memory");
    #endif
} 