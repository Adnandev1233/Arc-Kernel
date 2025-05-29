#ifndef ASM_H
#define ASM_H

#include <stdint.h>

// GCC inline assembly macros
#define ASM_VOLATILE(x) __asm__ __volatile__(x)
#define ASM(x) __asm__(x)

// Memory barrier
#define MEMORY_BARRIER() ASM_VOLATILE("" ::: "memory")

// CPU Control Registers
#define READ_CR0() ({ \
    uint32_t x; \
    __asm__ __volatile__("movl %%cr0, %0" : "=r" (x)); \
    x; \
})

#define WRITE_CR0(x) \
    __asm__ __volatile__("movl %0, %%cr0" : : "r" (x))

#define READ_CR2() ({ \
    uint32_t x; \
    __asm__ __volatile__("movl %%cr2, %0" : "=r" (x)); \
    x; \
})

#define WRITE_CR2(x) \
    __asm__ __volatile__("movl %0, %%cr2" : : "r" (x))

#define READ_CR3() ({ \
    uint32_t x; \
    __asm__ __volatile__("movl %%cr3, %0" : "=r" (x)); \
    x; \
})

#define WRITE_CR3(x) \
    __asm__ __volatile__("movl %0, %%cr3" : : "r" (x))

#define READ_CR4() ({ \
    uint32_t x; \
    __asm__ __volatile__("movl %%cr4, %0" : "=r" (x)); \
    x; \
})

#define WRITE_CR4(x) \
    __asm__ __volatile__("movl %0, %%cr4" : : "r" (x))

// CPU Flags
#define READ_EFLAGS() ({ \
    uint32_t x; \
    __asm__ __volatile__("pushfl; popl %0" : "=r" (x)); \
    x; \
})

#define WRITE_EFLAGS(x) \
    __asm__ __volatile__("pushl %0; popfl" : : "r" (x))

// CPU Instructions
#define CLI() \
    __asm__ __volatile__("cli")

#define STI() \
    __asm__ __volatile__("sti")

#define HLT() \
    __asm__ __volatile__("hlt")

#define NOP() \
    __asm__ __volatile__("nop")

// CPU Features
#define CPUID_FEATURES_EDX 0x00000001
#define CPUID_FEATURES_ECX 0x00000001

#define CPUID_FEATURE_PAE (1 << 6)
#define CPUID_FEATURE_PSE (1 << 3)
#define CPUID_FEATURE_PGE (1 << 13)

// CPUID instruction
static inline void cpuid(uint32_t leaf, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx) {
    __asm__ __volatile__("cpuid"
        : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
        : "a" (leaf));
}

#endif // ASM_H 