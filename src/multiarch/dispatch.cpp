
#include <cstddef>
#include <cstring>
#include <intrin.h>

#include <betterstring/detail/preprocessor.hpp>
#include <betterstring/detail/multiarch/strrfind_char_default.hpp>
#include <betterstring/detail/multiarch/strrfind_string_default.hpp>

namespace {
    BS_FORCEINLINE
    bool is_cpu_supports_avx2() {
        int cpuInfo[4];
        __cpuid(cpuInfo, 1);

        if ((cpuInfo[2] & (1 << 27)) && (cpuInfo[2] & (1 << 28))) {
            return (_xgetbv(0) & 0x6) == 0x6;
        }
        return false;
    }
}

namespace {
    const char* strrfind_str_resolver(const char* const haystack, const std::size_t count, const char* const needle, const std::size_t needle_len);
    const char* strrfind_ch_resolver(const char* const str, const std::size_t count, const char ch);

    const char*(*strrfind_ch_impl)(const char*, std::size_t, char) = &strrfind_ch_resolver;
    const char*(*strrfind_str_impl)(const char*, std::size_t, const char*, std::size_t) = &strrfind_str_resolver;
}

extern "C" const char* __cdecl betterstring_impl_strrfind_ch_avx2(const char*, std::size_t, char);

namespace bs::detail {
    extern const char* strrfind_string_avx2(const char*, std::size_t, const char*, std::size_t);

    const char* strrfind_str_runtime(const char* haystack, std::size_t count, const char* needle, std::size_t needle_len) {
        return strrfind_str_impl(haystack, count, needle, needle_len);
    }

    const char* strrfind_ch_runtime(const char* haystack, std::size_t count, char ch) {
#if BS_ARCH_AVX2
        return betterstring_impl_strrfind_ch_avx2(haystack, count, ch);
#else
        return strrfind_ch_impl(haystack, count, ch);
#endif
    }
}

namespace {
    const char* strrfind_str_resolver(const char* const haystack, const std::size_t count, const char* const needle, const std::size_t needle_len) {
        using namespace bs::detail;
        if (is_cpu_supports_avx2()) {
            //strrfind_impl = &strrfind_str_default;
            strrfind_str_impl = &strrfind_string_avx2;
        } else {
            strrfind_str_impl = &bs::detail::multiarch::strrfind_string_default;
        }
        return strrfind_str_impl(haystack, count, needle, needle_len);
    }

    const char* strrfind_ch_resolver(const char* const str, const std::size_t count, const char ch) {
        using namespace bs::detail;
        if (is_cpu_supports_avx2()) {
            //strrfind_ch_impl = &strrfind_ch_default;
            //strrfind_ch_impl = &strrfind_character_avx2;
            strrfind_ch_impl = &betterstring_impl_strrfind_ch_avx2;
        } else {
            strrfind_ch_impl = &bs::detail::multiarch::strrfind_char_default;
        }
        return strrfind_ch_impl(str, count, ch);
    }
}
