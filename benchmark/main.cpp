#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

#include <cstdint>
#include <charconv>
#include <map>
#include <functional>

#include "benchmarks.hpp"
#include "html_boxplot.hpp"

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif // _WIN32

#include <fmt/format.h>

#include <betterstring/parsing.hpp>
#include <betterstring/string_view.hpp>

namespace nb = ankerl::nanobench;

#pragma function(memcpy)

int main(int argc, char* argv[]) {
#ifdef _WIN32
    if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS)) {
        fmt::println("warning: failed to set process priority class");
    }
    if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL)) {
        fmt::println("warning: failed to set thread priority class");
    }
#endif // _WIN32
    nb::Bench bench;

    using namespace std::chrono_literals;
    bench.minEpochTime(260ms).warmup(100'000);

    const bs::string_view benchmark_name{argv[1], bs::strlen(argv[1])};
    std::optional benchmark_fn = register_benchmark::get_function(benchmark_name);
    if (!benchmark_fn.has_value()) {
        fmt::println("unknown benchmark name: {}", benchmark_name.data());
        return 1;
    }
    std::vector<bs::string_view<>> args;
    args.reserve(argc);
    for (std::size_t i = 2; i < argc; ++i) {
        args.push_back(bs::string_view{argv[i], bs::strlen(argv[i])});
    }
    (*benchmark_fn)(bench, args);

    bench.render(html_boxplot_template, std::cout);

    return 0;
}
