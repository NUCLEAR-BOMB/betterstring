#include <benchmark/benchmark.h>
#include <betterstring.hpp>

#include "fixture.hpp"

namespace {

static void printable_chars_args(benchmark::internal::Benchmark* const b) {
    for (char ch = 32; ch != 127; ++ch) {
        b->Arg(ch);
    }
}

// mean 40.609052631579 ns
// max 165 ns
// min 1.91 ns
BENCHMARK_DEFINE_F(string_fixture, strrfind)(benchmark::State& state) {
    const auto string = get_string();
    const auto size = get_string_size();
    for (auto _ : state) {
        benchmark::DoNotOptimize(bs::strrfind(string, size, char(state.range())));
    }
}
BENCHMARK_REGISTER_F(string_fixture, strrfind)->Apply(printable_chars_args);

// mean 615.36842105263 ns
// max 679 ns
// min 576 ns
BENCHMARK_DEFINE_F(string_fixture, strrchr)(benchmark::State& state) {
    const auto string = get_string();
    for (auto _ : state) {
        benchmark::DoNotOptimize(std::strrchr(string, char(state.range())));
    }
}
BENCHMARK_REGISTER_F(string_fixture, strrchr)->Apply(printable_chars_args);

BENCHMARK_F(string_fixture, strfind)(benchmark::State& state) {
    const auto string = get_string();
    const auto size = get_string_size();
    for (auto _ : state) {
        benchmark::DoNotOptimize(bs::strfind(string, size, char(state.range())));
    }
}
BENCHMARK_REGISTER_F(string_fixture, strfind)->Apply(printable_chars_args);

}
