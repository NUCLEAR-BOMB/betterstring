#include <benchmark/benchmark.h>
#include <betterstring.hpp>
#include <cstring>
#include <string>
#include <string_view>

#include "fixture.hpp"

namespace {

BENCHMARK_F(string_fixture, string_view_find)(benchmark::State& state) {
    const bs::string_view string = get_string();
    for (auto _ : state) {
        benchmark::DoNotOptimize(string.find("string"));
    }
}

BENCHMARK_F(string_fixture, std_strstr)(benchmark::State& state) {
    const auto string = get_string();
    for (auto _ : state) {
        benchmark::DoNotOptimize(std::strstr(string, "string"));
    }
}

BENCHMARK_F(string_fixture, std_string_find)(benchmark::State& state) {
    const std::string string = get_string();
    for (auto _ : state) {
        benchmark::DoNotOptimize(string.find("string"));
    }
}

BENCHMARK_F(string_fixture, std_string_view_find)(benchmark::State& state) {
    const std::string_view string = get_string();
    for (auto _ : state) {
        benchmark::DoNotOptimize(string.find("string"));
    }
}

}

