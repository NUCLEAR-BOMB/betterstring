#pragma once

#include <map>
#include <optional>
#include <betterstring/string_view.hpp>
#include <nanobench.h>

class register_benchmark {
    using benchmark_fn_type = void(*)(ankerl::nanobench::Bench&, const std::vector<bs::string_view>&);
    inline static std::map<bs::string_view, benchmark_fn_type> benchmark_registry;
public:
    register_benchmark(const char* benchmark_name, const benchmark_fn_type benchmark_fn) {
        const auto benchmark_name_str = bs::string_view{benchmark_name, bs::strlen(benchmark_name)};
        benchmark_registry.emplace(benchmark_name_str, benchmark_fn);
    }

    static std::optional<benchmark_fn_type> get_function(const bs::string_view benchmark_name) noexcept {
        if (auto fn = benchmark_registry.find(benchmark_name); fn != benchmark_registry.end()) {
            return fn->second;
        } else {
            return {};
        }
    }
};

#define CONCAT2(a, b) a##b
#define CONCAT(a, b) CONCAT2(a, b)

#define ADD_BENCHMARK_IMPL(name, benchmark_function_name) \
    static void benchmark_function_name(ankerl::nanobench::Bench&, const std::vector<bs::string_view>&); \
    namespace { const register_benchmark CONCAT(_benchmark_reg_, __COUNTER__){name, &benchmark_function_name}; } \
    static void benchmark_function_name(ankerl::nanobench::Bench& bench, const std::vector<bs::string_view>& args)

#define ADD_BENCHMARK(name) ADD_BENCHMARK_IMPL(name, CONCAT(benchmark_, __COUNTER__))
