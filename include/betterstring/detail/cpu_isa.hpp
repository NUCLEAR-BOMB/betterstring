// Copyright 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <betterstring/detail/preprocessor.hpp>

#if BS_COMP_MSVC
    #include <immintrin.h>
    #include <intrin.h>
#elif BS_COMP_GCC || BS_COMP_CLANG
    #include <cpuid.h>
    #include <immintrin.h>
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
#elif BS_COMP_GCC || BS_COMP_CLANG
BS_FORCEINLINE
inline cpuid_t cpuid(uint32_t leaf, uint32_t subleaf) {
    cpuid_t out;
    __cpuid_count(leaf, subleaf, out.eax, out.ebx, out.ecx, out.edx);
    return out;
}
BS_FORCEINLINE
inline uint64_t xgetbv(unsigned int a) {
    uint32_t eax = 0, edx = 0;
    __asm__ __volatile__("xgetbv\n" : "=a"(eax), "=d"(edx) : "c"(a));
    return ((uint64_t)edx << 32) | eax;
}
#endif

template<class T>
constexpr bool has_single_bit(const T x) noexcept {
    return (x ^ (x - 1)) > (x - 1);
}

struct alignas(64) cpu_features_t {
    uint64_t value{};
    uint64_t _dummy[7];
};

template<uint64_t Mask>
struct isa_tester {
    static constexpr uint64_t mask = Mask;

    constexpr operator bool() const noexcept;
};
template<uint64_t Mask1, uint64_t Mask2>
constexpr isa_tester<Mask1 | Mask2> operator&(const isa_tester<Mask1>, const isa_tester<Mask2>) noexcept {
    return {};
}

namespace isa {
    inline constexpr isa_tester<(1 << 0)> AVX2;
    inline constexpr isa_tester<(1 << 1)> BMI2;
    inline constexpr isa_tester<(1 << 2)> POPCNT;
}

// CPUID:
// https://en.wikipedia.org/wiki/CPUID
// https://www.felixcloutier.com/x86/cpuid

// XCR0:
// https://en.wikipedia.org/wiki/Control_register#XCR0_and_XSS

inline cpu_features_t dynamic_cpu_features_initializer() noexcept {
    cpuid_t regs;
    regs = cpuid(0x1, 0x0);

    const bool osxsave = regs.ecx & (1 << 27); // is cpu support xgetbv instruction
    const bool popcnt = regs.ecx & (1 << 23);

    regs = cpuid(0x7, 0x0);
    const bool avx2 = regs.ebx & (1 << 5);
    const bool bmi2 = regs.ebx & (1 << 8);

    cpu_features_t features{};
    features.value |= bmi2 ? isa::BMI2.mask : 0;
    features.value |= popcnt ? isa::POPCNT.mask : 0;

    if (osxsave && avx2) {
        uint64_t xcr0 = xgetbv(BS_XFEATURE_ENABLED_MASK);
        const bool os_sse = xcr0 & (1 << 1); // XMM regs
        const bool os_avx = xcr0 & (1 << 2); // YMM regs
        if (os_sse && os_avx) {
            features.value |= avx2 ? isa::AVX2.mask : 0;
        }
    }
    return features;
}

inline const cpu_features_t cpu_features = dynamic_cpu_features_initializer();

template<uint64_t Mask>
constexpr isa_tester<Mask>::operator bool() const noexcept {
    if constexpr (has_single_bit(Mask)) {
        return (cpu_features.value & Mask) != 0;
    } else {
        return ((~cpu_features.value) & Mask) == 0;
    }
}

}
