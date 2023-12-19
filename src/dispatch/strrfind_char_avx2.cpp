#include <betterstring/detail/dispatch.hpp>
#include <immintrin.h>

namespace bs::detail {
    BS_FLATTEN
    const char* strrfind_character_avx2(const char* str, std::size_t count, const char ch) {
        std::int64_t offset = 0;
        std::uint32_t mask;
        unsigned long match_index;
        __m256i block;

        __m256i ch_block = _mm256_set1_epi8(ch);

        if (count < 32) {
            goto fallback;
        }
        offset = reinterpret_cast<std::intptr_t>(str) & 31;
        if (offset == 0) {
            goto general;
        }
        count -= offset;
        str += offset;
        offset -= 32;
        while (offset != 0) {
            if (ch == *(str + offset)) {
                goto normal_end;
            }
            ++offset;
        }
        if (count < 32) {
            goto fallback;
        }
    general:
        do {
            block = _mm256_load_si256(reinterpret_cast<const __m256i*>(str));
            block = _mm256_cmpeq_epi8(block, ch_block);
            mask = static_cast<std::uint32_t>(_mm256_movemask_epi8(block));
            if (_BitScanForward(&match_index, mask) != 0) {
                goto normal_end;
            }
            str += 32;
            count -= 32;
        } while (count >= 32);
    fallback:
        while (count != 0) {
            if (ch == *str) {
                goto normal_end;
            }
            ++str;
            --count;
        }
        return nullptr;
    normal_end:
        return str + count;
    }
}
