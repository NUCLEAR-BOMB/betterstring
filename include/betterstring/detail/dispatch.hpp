#pragma once

#include <betterstring/detail/preprocessor.hpp>
#include <intrin.h>

template<class T>
BS_FORCEINLINE
inline unsigned int lzcnt(const T x) noexcept {
#if BS_COMP_GCC || BS_COMP_CLANG
    if constexpr (sizeof(T) <= sizeof(unsigned int)) {
        return unsigned(__builtin_clz(x));
    } else if constexpr (sizeof(T) <= sizeof(unsigned long)) {
        return unsigned(__builtin_clzl(x));
    } else {
        return unsigned(__builtin_clzll(x));
    }
#else
    if constexpr (sizeof(T) <= sizeof(unsigned int)) {
        return _lzcnt_u32(x);
    } else {
        return _lzcnt_u64(x);
    }
#endif
}

BS_FORCEINLINE
inline bool is_cpu_supports_avx2() {
#if BS_ARCH_AVX2
    int cpuInfo[4];
    __cpuid(cpuInfo, 1);

    if ((cpuInfo[2] & (1 << 27)) && (cpuInfo[2] & (1 << 28))) {
        return (_xgetbv(0) & 0x6) == 0x6;
    }
    return false;
#else
    return true;
#endif
}
