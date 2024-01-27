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

namespace nb = ankerl::nanobench;

int main() {
#ifdef _WIN32
    if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS)) {
        fmt::println("warning: failed to set process priority class");
    }
    if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL)) {
        fmt::println("warning: failed to set thread priority class");
    }
#endif // _WIN32

    nb::Bench bench;

    // benchmark_strrfind_character(bench);
    // benchmark_strrfind_string(bench);
    // benchmark_parse_u8(bench);
    // benchmark_parse_u16(bench);
    // benchmark_parse_u32(bench);
    // benchmark_parse_u64(bench);
}
