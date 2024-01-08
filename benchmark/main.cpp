#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>
#include <iostream>
#include <string>
#include <cstdint>
#include <charconv>

#include <betterstring/functions.hpp>
#include <betterstring/parsing.hpp>

#include "samples.hpp"

namespace nb = ankerl::nanobench;

int main() {
    nb::Bench bench;
    bench.warmup(100'000).minEpochIterations(20'000'000);
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
#if 0
    bench.warmup(100'000).minEpochIterations(200'000'000);
    bench.run("parse<uint8> (3)", [offset=0u]() mutable {
        auto result = bs::parse<uint8_t>(strings::NUMBERS + offset, 3);
        nb::doNotOptimizeAway(result);
        offset += 5;
        if (offset >= std::size(strings::NUMBERS) - 3) { offset = 0; }
    });
    bench.run("parse<uint8> (2)", [offset=0u]() mutable {
        auto result = bs::parse<uint8_t>(strings::NUMBERS + offset, 2);
        nb::doNotOptimizeAway(result);
        offset += 5;
        if (offset >= std::size(strings::NUMBERS) - 2) { offset = 0; }
    });
    bench.run("parse<uint8> (1)", [offset=0u]() mutable {
        auto result = bs::parse<uint8_t>(strings::NUMBERS + offset, 1);
        nb::doNotOptimizeAway(result);
        offset += 5;
        if (offset >= std::size(strings::NUMBERS) - 1) { offset = 0; }
    });
#endif
#if 0
    bench.warmup(100'000).minEpochIterations(100'000'000);
    bench.run("parse<uint16> (5)", [offset=0u]() mutable {
        auto result = bs::parse<uint16_t>(strings::NUMBERS + offset, 5);
        nb::doNotOptimizeAway(result);
        offset += 11;
        if (offset >= std::size(strings::NUMBERS) - 5) { offset = 0; }
    });
#endif
#if 0
    bench.warmup(100'000).minEpochIterations(200'000'000);
    bench.run("parse<uint32> (8)", [offset=0u]() mutable {
        auto result = bs::parse<uint32_t>(strings::NUMBERS + offset, 8);
        nb::doNotOptimizeAway(result);
        offset += 8;
        if (offset >= std::size(strings::NUMBERS) - 8) { offset = 0; }
    });
    bench.run("std::from_chars (8)", [offset = 0u]() mutable {
        uint32_t res;
        auto err = std::from_chars(strings::NUMBERS, strings::NUMBERS + 8, res);
        nb::doNotOptimizeAway(res);
        nb::doNotOptimizeAway(err);
        offset += 8;
        if (offset >= std::size(strings::NUMBERS) - 8) { offset = 0; }
    });
#endif
#if 0
    bench.warmup(100'000).minEpochIterations(250'000'000);
    bench.run("parse<uint16> (4)", [offset = 0u]() mutable {
        auto result = bs::parse<uint16_t>(strings::NUMBERS + offset, 4);
        nb::doNotOptimizeAway(result);
        offset += 4;
        if (offset >= std::size(strings::NUMBERS) - 4) { offset = 0; }
    });
    bench.run("parse<uint32> (4)", [offset = 0u]() mutable {
        auto result = bs::parse<uint32_t>(strings::NUMBERS + offset, 4);
        nb::doNotOptimizeAway(result);
        offset += 4;
        if (offset >= std::size(strings::NUMBERS) - 4) {
            offset = 0;
        }
    });
#endif
#if 0
    bench.warmup(100'000).minEpochIterations(250'000'000);
    bench.run("parse<uint8> (2)", [offset = 0u]() mutable {
        auto result = bs::parse<uint8_t>(strings::NUMBERS + offset, 2);
        nb::doNotOptimizeAway(result);
        offset += 2;
        if (offset >= std::size(strings::NUMBERS) - 2) { offset = 0; }
    });
    bench.run("parse<uint16> (2)", [offset = 0u]() mutable {
        auto result = bs::parse<uint16_t>(strings::NUMBERS + offset, 2);
        nb::doNotOptimizeAway(result);
        offset += 2;
        if (offset >= std::size(strings::NUMBERS) - 2) { offset = 0; }
    });
    bench.run("parse<uint32> (2)", [offset = 0u]() mutable {
        auto result = bs::parse<uint8_t>(strings::NUMBERS + offset, 2);
        nb::doNotOptimizeAway(result);
        offset += 2;
        if (offset >= std::size(strings::NUMBERS) - 2) { offset = 0; }
    });
#endif
#if 0
    bench.warmup(100'000).minEpochIterations(250'000'000);
    bench.run("parse<uint8> (3)", [offset = 0u]() mutable {
        auto result = bs::parse<uint8_t>(strings::NUMBERS + offset, 3);
        nb::doNotOptimizeAway(result);
        offset += 3;
        if (offset >= std::size(strings::NUMBERS) - 2) { offset = 0; }
    });
#endif
}
