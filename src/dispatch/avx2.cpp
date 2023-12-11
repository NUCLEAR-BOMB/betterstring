#include <cstddef>
#include <cstdint>
#include <cstring>
#include <intrin.h>

#include <betterstring/detail/preprocessor.hpp>

namespace {
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
}

namespace bs::detail {
    // https://stackoverflow.com/a/25884902
    // https://www.reddit.com/r/cpp/comments/weegza/comment/iinrhei/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button

    BS_FLATTEN
    const char* strrfind_string_avx2(const char* const str, const std::size_t count, const char* const needle, const std::size_t needle_len) {
        if (needle_len > count) return nullptr;
        if (needle_len == 0) return str + count;

        const char* char_ptr = str + (count - needle_len) + 1;

        while (char_ptr >= str + sizeof(__m256)) {
            char_ptr -= sizeof(__m256);

            const __m256i loaded = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(char_ptr));
            const __m256i cmp = _mm256_cmpeq_epi8(loaded, _mm256_set1_epi8(needle[0]));
            std::uint32_t cmp_mask = static_cast<std::uint32_t>(_mm256_movemask_epi8(cmp));

            for (const char* match_ptr = char_ptr + sizeof(__m256); cmp_mask != 0;) {
                const unsigned lzcnt_result = lzcnt(cmp_mask) + 1;

                cmp_mask = std::uint32_t(std::uint64_t(cmp_mask) << lzcnt_result);

                match_ptr -= lzcnt_result;
                if (std::memcmp(match_ptr, needle, needle_len) == 0) {
                    return match_ptr;
                }
            }
        }
        for (; char_ptr != str;) {
            --char_ptr;
            if (std::memcmp(char_ptr, needle, needle_len) == 0) {
                return char_ptr;
            }
        }
        return nullptr;
    }
}
