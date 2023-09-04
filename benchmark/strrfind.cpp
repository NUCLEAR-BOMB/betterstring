#include <benchmark/benchmark.h>
#include <betterstring.hpp>

#ifdef __AVX2__

#include <immintrin.h>

namespace {

static constexpr const char* const SEARCH_STRING = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc";
static constexpr std::size_t SEARCH_STRING_SIZE = bs::strlen(SEARCH_STRING);



template<class T>
const T* avx2_strrfind(const T* string, const T search_character, std::size_t count) {
    const T* char_ptr = string + count - 1;
    // align
    for (; count > 0 && ((reinterpret_cast<std::uintptr_t>(char_ptr) & (sizeof(__m256i) - 1)) != 0)
        ; --count, --char_ptr) {
        if (*char_ptr == search_character) return char_ptr;
    }
    const __m256i search_char8 = _mm256_set1_epi8(search_character);
    for (; count >= sizeof(__m256i); count -= sizeof(__m256i)) {
        char_ptr -= sizeof(__m256i);
        const __m256i char8 = _mm256_load_si256(reinterpret_cast<const __m256i*>(char_ptr));
        const __m256i cmpeq_result = _mm256_cmpeq_epi8(char8, search_char8);
        const std::int32_t cmp_mask = _mm256_movemask_epi8(cmpeq_result);
        if (cmp_mask != 0) {
            return &char_ptr[_tzcnt_u32(static_cast<unsigned int>(cmp_mask))];
        }
    }
    for (; count != 0; --count, --char_ptr) {
        if (*char_ptr == search_character) return char_ptr;
    }
    return nullptr;
}

void BM_avx2_strrfind(benchmark::State& state) {
    char search_ch = 'b';
    for (auto _ : state) {
        benchmark::DoNotOptimize(search_ch);
        benchmark::DoNotOptimize(avx2_strrfind(SEARCH_STRING, search_ch, SEARCH_STRING_SIZE));
    }
}
BENCHMARK(BM_avx2_strrfind);

template<class T>
const T* const simple_strrfind(const T* const s, const T ch, const std::size_t count) {
    for (auto it = s + count - 1;; --it) {
        if (*it == ch) return it;
        if (it == s) break;
    }
    return nullptr;
}

void BM_simple_strrfind(benchmark::State& state) {
    char search_ch = 'b';
    for (auto _ : state) {
        benchmark::DoNotOptimize(search_ch);
        benchmark::DoNotOptimize(simple_strrfind(SEARCH_STRING, search_ch, SEARCH_STRING_SIZE ));
    }
}
BENCHMARK(BM_simple_strrfind);

}

#endif
