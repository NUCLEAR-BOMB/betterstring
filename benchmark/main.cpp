#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>
#include <iostream>
#include <string>

#include <betterstring/functions.hpp>

#include "samples.hpp"

namespace nb = ankerl::nanobench;

int main() {
    nb::Bench bench;
    bench.warmup(200'000).minEpochIterations(100'000);

#if 0
    bench.run("random (large needle)", [&] {
        auto result = bs::strrfind(strings::RANDOM, std::size(strings::RANDOM), R"(R#MjFR7d-)wMJRCVD0gft!5h9TWUybW>OQST;|\TStoqi3k)");
        nb::doNotOptimizeAway(result);
    });
    bench.run("random (small needle)", [&] {
        auto result = bs::strrfind(strings::RANDOM, std::size(strings::RANDOM), R"(\TStoqi3k)");
        nb::doNotOptimizeAway(result);
    });
    bench.run("homogeneous (large needle)", [&] {
        auto result = bs::strrfind(strings::HOMOGENEOUS, std::size(strings::HOMOGENEOUS), "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
        nb::doNotOptimizeAway(result);
    });
    bench.run("homogeneous (small needle)", [&] {
        auto result = bs::strrfind(strings::HOMOGENEOUS, std::size(strings::HOMOGENEOUS), R"(bbbbbbbbbb)");
        nb::doNotOptimizeAway(result);
    });
#endif
#if 1
    bench.run("character", [&] {
        auto result = bs::strrfind(strings::HOMOGENEOUS, std::size(strings::HOMOGENEOUS), 'b');
        nb::doNotOptimizeAway(result);
    });
#endif
}
