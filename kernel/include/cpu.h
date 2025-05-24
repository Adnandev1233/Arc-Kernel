#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stddef.h>

// CPU feature flags
#define CPU_FEATURE_SSE     (1 << 0)
#define CPU_FEATURE_SSE2    (1 << 1)
#define CPU_FEATURE_SSE3    (1 << 2)
#define CPU_FEATURE_SSSE3   (1 << 3)
#define CPU_FEATURE_SSE4_1  (1 << 4)
#define CPU_FEATURE_SSE4_2  (1 << 5)
#define CPU_FEATURE_AVX     (1 << 6)
#define CPU_FEATURE_AVX2    (1 << 7)

// CPU information structure
typedef struct {
    uint32_t vendor_id[3];    // CPU vendor ID
    uint32_t features;        // CPU features
    uint32_t family;          // CPU family
    uint32_t model;           // CPU model
    uint32_t stepping;        // CPU stepping
    char vendor_string[13];   // CPU vendor string
} cpu_info_t;

// Function declarations
void cpu_init(void);
void cpu_get_info(cpu_info_t* info);
uint32_t cpu_get_features(void);
void cpu_enable_features(void);

// Inline functions for CPU control
#define cpu_halt() __builtin_ia32_hlt()
#define cpu_cli() __builtin_ia32_cli()
#define cpu_sti() __builtin_ia32_sti()

static inline uint32_t cpu_get_flags(void) {
    uint32_t flags;
    #ifdef __GNUC__
    __asm__ __volatile__("pushf\n\t"
                        "pop %0"
                        : "=r" (flags)
                        :
                        : "memory");
    #endif
    return flags;
}

static inline void cpu_set_flags(uint32_t flags) {
    #ifdef __GNUC__
    __asm__ __volatile__("push %0\n\t"
                        "popf"
                        :
                        : "r" (flags)
                        : "memory");
    #endif
}

#endif // CPU_H 