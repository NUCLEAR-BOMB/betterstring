#include <benchmark/benchmark.h>
#include <betterstring.hpp>

#ifdef __AVX2__

#include <immintrin.h>

namespace {

static constexpr const char* const SEARCH_STRING = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc";
static constexpr std::size_t SEARCH_STRING_SIZE = bs::strlen(SEARCH_STRING);

void BM_avx2_strrfind(benchmark::State& state) {
    char search_ch = 'b';
    for (auto _ : state) {
        benchmark::DoNotOptimize(search_ch);
        benchmark::DoNotOptimize(bs::strrfind(SEARCH_STRING, SEARCH_STRING_SIZE, search_ch));
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
