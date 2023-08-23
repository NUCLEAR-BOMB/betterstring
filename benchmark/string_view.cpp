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

BENCHMARK_F(string_fixture, std_strstr_find)(benchmark::State& state) {
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

BENCHMARK_F(string_fixture, string_view_rfind)(benchmark::State& state) {
    const bs::string_view string = get_string();
    for (auto _ : state) {
        benchmark::DoNotOptimize(string.rfind("string"));
    }
}

BENCHMARK_F(string_fixture, std_string_view_rfind)(benchmark::State& state) {
    const std::string_view string = get_string();
    for (auto _ : state) {
        benchmark::DoNotOptimize(string.rfind("string"));
    }
}

BENCHMARK_F(string_fixture, std_string_rfind)(benchmark::State& state) {
    const std::string string = get_string();
    for (auto _ : state) {
        benchmark::DoNotOptimize(string.rfind("string"));
    }
}

#if 0
BENCHMARK_F(string_fixture, string_view_strlen)(benchmark::State& state) {
    for (auto _ : state) {
        bs::string_view string(get_string());
        benchmark::DoNotOptimize(string);
    }
}

BENCHMARK_F(string_fixture, std_string_view_strlen)(benchmark::State& state) {
    for (auto _ : state) {
        std::string_view string(get_string());
        benchmark::DoNotOptimize(string);
    }
}

BENCHMARK_F(string_fixture, std_strlen)(benchmark::State& state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(std::strlen(get_string()));
    }
}
#endif

}

