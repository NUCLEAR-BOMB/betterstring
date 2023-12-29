#pragma once

#include <cstddef>
#include <cstdint>
#include <immintrin.h>
#include <intrin.h>

#include <betterstring/detail/preprocessor.hpp>

namespace bs::detail::multiarch {
    
    inline const char* strrfind_char_avx2(const char* str, std::size_t count, const char ch) {
        static_assert(sizeof(unsigned long) == 4);
        static_assert(sizeof(__m256i) == 32);

        //__debugbreak();
        str += count;
        if (count >= 32) {
            std::int64_t align_offset = reinterpret_cast<std::intptr_t>(str) & 31;
            count -= align_offset;
            str -= align_offset;
            --str;
            do {
                if (*(str + align_offset) == ch) { return str + align_offset; }
            } while (--align_offset != 0);
            ++str;
        }
        const __m256i ch_block = _mm256_set1_epi8(ch);
        while (count >= 32) {
            str -= 32;
            const __m256i str_block = _mm256_load_si256(reinterpret_cast<const __m256i*>(str));
            const __m256i cmp = _mm256_cmpeq_epi8(ch_block, str_block);
            const unsigned long cmp_mask = static_cast<std::uint32_t>(_mm256_movemask_epi8(cmp));
            unsigned long match_index;
            if (_BitScanReverse(&match_index, cmp_mask) != 0) {
                return match_index + str;
            }
            count -= 32;
        }
        for (; count != 0; --count) {
             --str;
            if (*str == ch) { return str; }
        }
        return nullptr;
    }
}
