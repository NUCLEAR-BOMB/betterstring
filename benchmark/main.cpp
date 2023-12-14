#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>
#include <iostream>
#include <string>

#include <betterstring/functions.hpp>

#include <fmt/format.h>

#include "samples.hpp"

namespace nb = ankerl::nanobench;

int main() {
    nb::Bench bench;
    bench.minEpochIterations(10'000)
         .minEpochTime(std::chrono::milliseconds(1000));

    for (std::size_t search_string_len : {10, 100, 1000, 10'000, 40'000}) {
        const char* const search_string = SAMPLE_STRING_50000 + 200;

        for (std::size_t sample_string_len : {50'000, 10'000, 5000, 1000, 100}) {
            if (sample_string_len <= search_string_len) continue;

            const auto name = fmt::format("strrfind [{}] [{}]", sample_string_len, search_string_len);
            bench.run(name, [&] {
                auto result = bs::strrfind(SAMPLE_STRING_50000, sample_string_len, search_string, search_string_len);
                nb::doNotOptimizeAway(result);
            });
        }
    }
}
