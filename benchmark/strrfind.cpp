#include <benchmark/benchmark.h>
#include <betterstring.hpp>

#ifdef __AVX2__

#include <immintrin.h>

namespace {

constexpr const char* const SEARCH_STRING = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc";
constexpr std::size_t SEARCH_STRING_SIZE = bs::strlen(SEARCH_STRING);

void BM_avx2_strrfind(benchmark::State& state) {
    char search_ch = 'b';
    for (auto _ : state) {
        benchmark::DoNotOptimize(search_ch);
        benchmark::DoNotOptimize(bs::strrfind(SEARCH_STRING, SEARCH_STRING_SIZE, search_ch));
    }
}
BENCHMARK(BM_avx2_strrfind)->Name("avx2_strrfind_character");

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
BENCHMARK(BM_simple_strrfind)->Name("simple_strrfind_character");

constexpr const char* const SEARCH_STRING2 = "aaaaaaaaaaaaaa12345678aaaaaaaaaaaaaaaa1aaassdsaaaaaaaaaaaaaaaaa1aaaaaaeeeeeeeeeee1ahhhhjkkkkkkeeeeeeeeeeeeeeeee1eeeeeeee3eeeeeeeeeeeeeeeeeee4eeeeeeeeaa2aaaaaaaaaaaaaaaaaahhhh1hhhhhhhhhhhhhhhhhhhhhhh";
constexpr std::size_t SEARCH_STRING2_SIZE = bs::strlen(SEARCH_STRING2);

void BM_strrfind_string(benchmark::State& state) {
    for (auto _ : state) {
        auto result = bs::strrfind(SEARCH_STRING2, std::size_t(state.range(0)), "12345678", 8);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_strrfind_string)
    ->Name("avx2_strrfind_string")
    ->DenseRange(/*start=*/20, /*stop=*/60, /*step=*/2);

template<class T>
const T* simple_strrfind_string(const T* const str, const std::size_t count, const T* const needle, const std::size_t needle_len) noexcept {
    if (needle_len > count) return nullptr;
    for (auto it = str + (count - needle_len);; --it) {
        for (std::size_t i = 0; i < needle_len; ++i) {
            if (it[i] == needle[i]) return it;
        }
        if (it == str) break;
    }
    return nullptr;
}

#if 0
void BM_simple_strrfind_string(benchmark::State& state) {
    for (auto _ : state) {
        auto result = simple_strrfind_string(SEARCH_STRING2, std::size_t(state.range(0)), "12345678", 8);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_simple_strrfind_string)
    ->Name("simple_strrfind_string")
    ->DenseRange(25, SEARCH_STRING2_SIZE, 10);
#endif

template<class T>
const T* with_memcmp_strrfind_string(const T* const str, const std::size_t count, const T* const needle, const std::size_t needle_len) noexcept {
    if (needle_len > count) return nullptr;
    if (needle_len == 0) return str;

    for (auto it = str + (count - needle_len);; --it) {
        if (std::memcmp(it, needle, needle_len) == 0) return it;
        if (it == str) break;
    }
    return nullptr;
}
void BM_simple_memcmp_strrfind_string(benchmark::State& state) {
    for (auto _ : state) {
        auto result = with_memcmp_strrfind_string(SEARCH_STRING2, std::size_t(state.range(0)), "12345678", 8);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_simple_memcmp_strrfind_string)
    ->Name("with_memcmp_strrfind_string")
    ->DenseRange(/*start=*/20, /*stop=*/60, /*step=*/2);

#if 0
template<class T>
const T* with_strrfind_ch_strrfind_string(const T* const str, const std::size_t count, const T* const needle, const std::size_t needle_len) noexcept {
    if (needle_len > count) return nullptr;
    if (needle_len == 0) return str + count;

    for (auto match_try = str + (count - needle_len);; --match_try) {
        match_try = bs::strrfind(str, std::size_t(match_try - str) + 1, needle[0]);
        if (match_try == nullptr) break;
        if (std::memcmp(match_try, needle, needle_len) == 0) {
            return match_try;
        }
    }
    return nullptr;
}

void BM_with_strrfind_ch_strrfind_string(benchmark::State& state) {
    for (auto _ : state) {
        auto result = with_strrfind_ch_strrfind_string(SEARCH_STRING2, std::size_t(state.range(0)), "12345678", 8);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_with_strrfind_ch_strrfind_string)
    ->Name("with_strrfind_ch_strrfind_string")
    ->DenseRange(/*start=*/25, /*stop=*/60, /*step=*/5);

#endif
}

#endif
