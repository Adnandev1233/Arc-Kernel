#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>

// CPU information structure
typedef struct {
    uint32_t vendor_id[3];
    char vendor_string[13];
    uint32_t features;
    uint8_t family;
    uint8_t model;
    uint8_t stepping;
} cpu_info_t;

// CPU feature flags
#define CPU_FEATURE_FPU     (1 << 0)
#define CPU_FEATURE_VME     (1 << 1)
#define CPU_FEATURE_DE      (1 << 2)
#define CPU_FEATURE_PSE     (1 << 3)
#define CPU_FEATURE_TSC     (1 << 4)
#define CPU_FEATURE_MSR     (1 << 5)
#define CPU_FEATURE_PAE     (1 << 6)
#define CPU_FEATURE_MCE     (1 << 7)
#define CPU_FEATURE_CX8     (1 << 8)
#define CPU_FEATURE_APIC    (1 << 9)
#define CPU_FEATURE_SEP     (1 << 11)
#define CPU_FEATURE_MTRR    (1 << 12)
#define CPU_FEATURE_PGE     (1 << 13)
#define CPU_FEATURE_MCA     (1 << 14)
#define CPU_FEATURE_CMOV    (1 << 15)
#define CPU_FEATURE_PAT     (1 << 16)
#define CPU_FEATURE_PSE36   (1 << 17)
#define CPU_FEATURE_PSN     (1 << 18)
#define CPU_FEATURE_CLFLUSH (1 << 19)
#define CPU_FEATURE_DS      (1 << 21)
#define CPU_FEATURE_ACPI    (1 << 22)
#define CPU_FEATURE_MMX     (1 << 23)
#define CPU_FEATURE_FXSR    (1 << 24)
#define CPU_FEATURE_SSE     (1 << 25)
#define CPU_FEATURE_SSE2    (1 << 26)
#define CPU_FEATURE_SS      (1 << 27)
#define CPU_FEATURE_HTT     (1 << 28)
#define CPU_FEATURE_TM      (1 << 29)
#define CPU_FEATURE_IA64    (1 << 30)
#define CPU_FEATURE_PBE     (1 << 31)

// CPU initialization
void cpu_init(void);

// Get CPU information
void cpu_get_info(cpu_info_t* info);

// Get CPU features
uint32_t cpu_get_features(void);

// Check if CPU has specific feature
bool cpu_has_feature(uint32_t feature);

// Enable CPU features
void cpu_enable_features(void);

// Enable interrupts
void enable_interrupts(void);

// Disable interrupts
void disable_interrupts(void);

// Halt CPU
void halt_cpu(void);

// Enable paging
void enable_paging(uint64_t* page_dir);

// Disable paging
void disable_paging(void);

// Load page directory
void load_page_directory(uint32_t* page_directory);

// Flush TLB
void flush_tlb(void);

#endif // CPU_H 