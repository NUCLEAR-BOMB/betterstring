#include <intrin.h>

#include <betterstring/detail/preprocessor.hpp>

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
