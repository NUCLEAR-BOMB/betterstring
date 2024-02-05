#pragma once

#include <betterstring/detail/preprocessor.hpp>

#if BS_COMP_MSVC
    #include <immintrin.h>
    #include <intrin.h>
#elif BS_COMP_GCC
    #include <cpuid.h>
    #include <xsaveintrin.h>
#endif

#ifdef _XCR_XFEATURE_ENABLED_MASK
    #define BS_XFEATURE_ENABLED_MASK _XCR_XFEATURE_ENABLED_MASK
#else
    #define BS_XFEATURE_ENABLED_MASK 0x0
#endif

namespace bs::detail {

struct cpuid_t {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
};

#if BS_COMP_MSVC
BS_FORCEINLINE
inline cpuid_t cpuid(uint32_t leaf, uint32_t subleaf) {
    cpuid_t out;
    uint32_t info[4];
    __cpuidex((int*)info, (int)leaf, (int)subleaf);
    out.eax = info[0];
    out.ebx = info[1];
    out.ecx = info[2];
    out.edx = info[3];
    return out;
}
BS_FORCEINLINE
inline uint64_t xgetbv(unsigned int a) {
    return _xgetbv(a);
}
#elif BS_COMP_GCC
BS_FORCEINLINE
inline cpuid_t cpuid(uint32_t leaf, uint32_t subleaf) {
    cpuid_t out;
    __cpuid_count(leaf, subleaf, out.eax, out.ebx, out.ecx, out.edx);
    return out;
}
[[gnu::target("xsave")]] BS_FORCEINLINE
inline uint64_t xgetbv(unsigned int a) {
    return _xgetbv(a);
}
#endif

struct cpu_features_t {
    uint64_t value{};

    enum : uint64_t {
        AVX2 = (1 << 0),
        BMI2 = (1 << 1),
    };
};

// CPUID:
// https://en.wikipedia.org/wiki/CPUID
// https://www.felixcloutier.com/x86/cpuid

// XCR0:
// https://en.wikipedia.org/wiki/Control_register#XCR0_and_XSS

inline cpu_features_t dynamic_cpu_features_initializer() noexcept {
    cpuid_t regs;
    regs = cpuid(0x1, 0x0);

    const bool osxsave = regs.ecx & (1 << 27); // is cpu support xgetbv instruction

    regs = cpuid(0x7, 0x0);
    const bool avx2 = regs.ebx & (1 << 5);
    const bool bmi2 = regs.ebx & (1 << 8);

    cpu_features_t features{};
    features.value |= bmi2 ? features.BMI2 : 0;

    if (osxsave && avx2) {
        uint64_t xcr0 = xgetbv(BS_XFEATURE_ENABLED_MASK);
        const bool os_sse = xcr0 & (1 << 1); // XMM regs
        const bool os_avx = xcr0 & (1 << 2); // YMM regs
        if (os_sse && os_avx) {
            features.value |= avx2 ? features.AVX2 : 0;
        }
    }
    return features;
}

alignas(64)
inline const cpu_features_t cpu_features = dynamic_cpu_features_initializer();

}
