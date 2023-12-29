#pragma once

#include <betterstring/detail/preprocessor.hpp>
#include <immintrin.h>

#if BS_COMP_MSVC
    #include <intrin.h>
#endif

namespace bs::detail {
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

namespace bs::detail::multiarch {
    inline const char* strrfind_string_avx2(const char* const str, const std::size_t count, const char* const needle, const std::size_t needle_len) {
#if 0
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
#else
        if (needle_len > count) return nullptr;
        if (needle_len == 0) return str + count;

        const __m256i first = _mm256_set1_epi8(needle[0]);
        const __m256i last  = _mm256_set1_epi8(needle[needle_len - 1]);

        const char* str_ptr = str + count;
        while (str_ptr >= str) {
            str_ptr -= sizeof(__m256i);

            // Accesses string out of bounds?
            const __m256i block_first = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(str_ptr - needle_len + 1));
            const __m256i block_last = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(str_ptr));

            const __m256i eq_first = _mm256_cmpeq_epi8(block_first, first);
            const __m256i eq_last = _mm256_cmpeq_epi8(block_last, last);

            std::uint32_t mask = static_cast<std::uint32_t>(_mm256_movemask_epi8(_mm256_and_si256(eq_first, eq_last)));

            const char* const match_ptr = str_ptr - needle_len + 1 + sizeof(__m256i);
            while (mask != 0) {
                const unsigned match_pos = bs::detail::lzcnt(mask) + 1;

                //if (std::memcmp(match_ptr - match_pos + 1, needle + 1, needle_len - 2) == 0) {
                if (std::memcmp(match_ptr - match_pos, needle, needle_len) == 0) {
#if 1 // Possible more instructions can fit in the cache
                    if (match_ptr - match_pos >= str) {
                        return match_ptr - match_pos;
                    } else {
                        return nullptr;
                    }
                    /* cmp   rsi,qword ptr [str]
                       jb    bs::detail::strrfind_string_avx2+109h
                       mov   rax,rsi
                       jmp   bs::detail::strrfind_string_avx2+10Bh */
#else
                    return match_ptr - match_pos >= str ? match_ptr - match_pos : nullptr;
                    /* xor   eax,eax
                       cmp   rsi,qword ptr [str]
                       cmovb rsi,rax
                       mov   rax,rsi
                       jmp   bs::detail::strrfind_string_avx2+10Fh */
#endif
                }
                mask <<= match_pos;
                mask >>= match_pos;
            }
        }
        return nullptr;
#endif
    }
}
