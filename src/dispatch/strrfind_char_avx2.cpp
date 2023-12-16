#include <betterstring/detail/dispatch.hpp>
#include <immintrin.h>

namespace bs::detail {
    BS_FLATTEN
    const char* strrfind_character_avx2(const char* const str, const std::size_t count, const char ch) {
        if (count == 0) return nullptr;

        const __m256i loaded_ch = _mm256_set1_epi8(ch);

        const char* str_ptr = str + count;
        while (str_ptr >= str) {
            str_ptr -= sizeof(__m256i);

            const __m256i block = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(str_ptr));

            const __m256i eq_block = _mm256_cmpeq_epi8(block, loaded_ch);

            const std::uint32_t mask = static_cast<std::uint32_t>(_mm256_movemask_epi8(eq_block));

            if (mask != 0) {
                const char* const match_ptr = str_ptr + sizeof(__m256i) - lzcnt(mask) - sizeof(char);
                if (match_ptr >= str) {
                    return match_ptr;
                } else {
                    return nullptr;
                }
            }
        }
        return nullptr;
    }

    BS_FLATTEN
    const char* strrfind_char16_avx2(const char* const str, const std::size_t count, const char16_t ch) {
        if (count == 0) return nullptr;

        const __m256i loaded_ch = _mm256_set1_epi16(static_cast<std::int16_t>(ch));

        const char* str_ptr = str + count;
        while (str_ptr >= str) {
            str_ptr -= sizeof(__m256i);

            const __m256i block = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(str_ptr));

            const __m256i eq_block = _mm256_cmpeq_epi16(block, loaded_ch);

            const std::uint32_t mask = static_cast<std::uint32_t>(_mm256_movemask_epi8(eq_block));

            if (mask != 0) {
                const char* const match_ptr = str_ptr + sizeof(__m256i) - lzcnt(mask) - sizeof(char16_t);
                if (match_ptr >= str) {
                    return match_ptr;
                } else {
                    return nullptr;
                }
            }
        }
        return nullptr;
    }

    BS_FLATTEN
    const char* strrfind_char32_avx2(const char* const str, const std::size_t count, const char32_t ch) {
        if (count == 0) return nullptr;

        const __m256i loaded_ch = _mm256_set1_epi32(static_cast<std::int32_t>(ch));

        const char* str_ptr = str + count;
        while (str_ptr >= str) {
            str_ptr -= sizeof(__m256i);

            const __m256i block = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(str_ptr));

            const __m256i eq_block = _mm256_cmpeq_epi32(block, loaded_ch);

            const std::uint32_t mask = static_cast<std::uint32_t>(_mm256_movemask_epi8(eq_block));

            if (mask != 0) {
                const char* const match_ptr = str_ptr + sizeof(__m256i) - lzcnt(mask) - sizeof(char32_t);
                if (match_ptr >= str) {
                    return match_ptr;
                } else {
                    return nullptr;
                }
            }
        }
        return nullptr;
    }
}
