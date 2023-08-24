#include <benchmark/benchmark.h>
#include <betterstring.hpp>

#include "fixture.hpp"

namespace {

static void printable_chars_args(benchmark::internal::Benchmark* const b) {
    for (char ch = 32; ch != 127; ++ch) {
        b->Arg(ch);
    }
}

// MSVC:
// mean 40.609052631579 ns
// max  165 ns
// min  1.91 ns
BENCHMARK_DEFINE_F(string_fixture, strrfind)(benchmark::State& state) {
    const auto string = get_string();
    const auto size = get_string_size();
    for (auto _ : state) {
        benchmark::DoNotOptimize(bs::strrfind(string, size, char(state.range())));
    }
}
BENCHMARK_REGISTER_F(string_fixture, strrfind)->Apply(printable_chars_args);

// MSVC:
// mean 615.36842105263 ns
// max  679 ns
// min  576 ns
BENCHMARK_DEFINE_F(string_fixture, strrchr)(benchmark::State& state) {
    const auto string = get_string();
    for (auto _ : state) {
        benchmark::DoNotOptimize(std::strrchr(string, char(state.range())));
    }
}
BENCHMARK_REGISTER_F(string_fixture, strrchr)->Apply(printable_chars_args);


static void strings_args(benchmark::internal::Benchmark* const b) {
    std::int64_t num = 4611686018427387847LL;
    for (std::size_t i = 0; i < 20; ++i) {
        num = num * 1103515245 + 12345;
        b->Arg(num);
    }
}

// MSVC:
// mean 151.5 ns
// max 	846 ns
// min  145 ns
BENCHMARK_DEFINE_F(string_fixture, strfind_string)(benchmark::State& state) {
    const int64_t integer = state.range();
    const char* const needle = reinterpret_cast<const char*>(&integer);
    const char* const haystack = get_string();
    const size_t haystack_len = get_string_size();
    for (auto _ : state) {
        benchmark::DoNotOptimize(
            bs::strfind(haystack, haystack_len, needle, sizeof(integer))
        );
    }
}
BENCHMARK_REGISTER_F(string_fixture, strfind_string)->Apply(strings_args);

// MSVC:
// mean 511 ns
// max  584 ns
// min  500 ns
BENCHMARK_DEFINE_F(string_fixture, strstr)(benchmark::State& state) {
    const int64_t integer[2] = { state.range(), '\0' };
    const char* const needle = reinterpret_cast<const char*>(&integer);
    const char* const haystack = get_string();
    for (auto _ : state) {
        benchmark::DoNotOptimize(
            std::strstr(haystack, needle)
        );
    }
}
BENCHMARK_REGISTER_F(string_fixture, strstr)->Apply(strings_args);


}
