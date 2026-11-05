#include <stdio.h>
#include <stdint.h>

#if defined(__x86_64__) || defined(_M_X64)
    #include <immintrin.h>
    #include <cpuid.h>
#endif

/**
 * Helper function to check for BMI2 hardware support at runtime (GCC/Clang).
 * Must be defined above derive_source so the compiler knows it exists.
 */
static inline int has_bmi2_support(void) {
#if defined(__x86_64__) && defined(__GNUC__)
    unsigned int eax, ebx, ecx, edx;
    if (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx)) {
        return (ebx & (1 << 8)) != 0;
    }
#elif defined(_M_X64) && defined(_MSC_VER)
    int cpuInfo[4];
    __cpuidex(cpuInfo, 7, 0);
    return (cpuInfo[1] & (1 << 8)) != 0; 
#endif
    return 0;
}

/**
 * Generates a valid source value given a PEXT destination value and its mask.
 * Attributes tell GCC/Clang it is safe to inline BMI2 intrinsics here.
 */
#if defined(__GNUC__) && defined(__x86_64__)
__attribute__((target("bmi2")))
#endif
uint64_t derive_source(uint64_t dest, uint64_t mask) {
    // 1. If the hardware supports it, execute the blazing-fast PDEP intrinsic
#if defined(__x86_64__) || defined(_M_X64)
    if (has_bmi2_support()) {
        return _pdep_u64(dest, mask);
    }
#endif

    // 2. Safe software fallback loop for older CPUs or non-x86 platforms
    uint64_t source = 0;
    uint32_t dest_bit = 0;

    while (mask != 0) {
        uint64_t lowest_mask_bit = mask & -mask;
        
        if ((dest & (1ULL << dest_bit)) != 0) {
            source |= lowest_mask_bit;
        }
        
        dest_bit++;
        mask ^= lowest_mask_bit;
    }

    return source;
}

int main(void) {
    uint64_t mask = 0x25; // Binary: 0010 0101
    uint64_t dest = 0x06; // Binary: 0000 0110

    uint64_t valid_src = derive_source(dest, mask);
    
    printf("Derived Source: 0x%016llX\n", (unsigned long long)valid_src);
    return 0;
}
