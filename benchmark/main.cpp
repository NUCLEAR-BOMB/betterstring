#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

#include <cstdint>
#include <charconv>

#include "benchmarks.hpp"

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif // _WIN32

#include <fmt/format.h>

#include <betterstring/parsing.hpp>
#include <betterstring/string_view.hpp>

namespace nb = ankerl::nanobench;

int main(int argc, char* argv[]) {
#ifdef _WIN32
    if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS)) {
        fmt::println("warning: failed to set process priority class");
    }
    if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL)) {
        fmt::println("warning: failed to set thread priority class");
    }
#endif // _WIN32

    if (argc <= 1) {
        fmt::println("select the name benchmark: "
            "[strrfind_ch,strrfind_str,parse_u8,parse_u16,parse_u32,parse_u64]");
        return -1;
    }
    nb::Bench bench;

    const bs::string_view benchmark_name{argv[1], bs::strlen(argv[1])};
    if (benchmark_name == "strrfind_ch") {
        benchmark_strrfind_character(bench);
    } else if (benchmark_name == "strrfind_str") {
        benchmark_strrfind_string(bench);
    } else if (benchmark_name == "parse_u8") {
        benchmark_parse_u8(bench);
    } else if (benchmark_name == "parse_u16") {
        benchmark_parse_u16(bench);
    } else if (benchmark_name == "parse_u32") {
        benchmark_parse_u32(bench);
    } else if (benchmark_name == "parse_u64") {
        benchmark_parse_u64(bench);
    } else {
        fmt::println("unknown benchmark name: {}", benchmark_name.data());
        return -1;
    }
    return 0;
}
