#pragma once

#include <nanobench.h>

#include <betterstring/functions.hpp>
#include <betterstring/parsing.hpp>

#include <fmt/format.h>

#include "samples.hpp"

inline void benchmark_strrfind_string(ankerl::nanobench::Bench& bench) {
    using ankerl::nanobench::doNotOptimizeAway;
    bench.run("random (large needle)", [&] {
        auto result = bs::strrfind(strings::RANDOM, std::size(strings::RANDOM), R"(R#MjFR7d-)wMJRCVD0gft!5h9TWUybW>OQST;|\TStoqi3k)");
        doNotOptimizeAway(result);
    });
    bench.run("random (small needle)", [&] {
        auto result = bs::strrfind(strings::RANDOM, std::size(strings::RANDOM), R"(\TStoqi3k)");
        doNotOptimizeAway(result);
    });
    bench.run("homogeneous (large needle)", [&] {
        auto result = bs::strrfind(strings::HOMOGENEOUS, std::size(strings::HOMOGENEOUS), "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
        doNotOptimizeAway(result);
    });
    bench.run("homogeneous (small needle)", [&] {
        auto result = bs::strrfind(strings::HOMOGENEOUS, std::size(strings::HOMOGENEOUS), R"(bbbbbbbbbb)");
        doNotOptimizeAway(result);
    });
}

inline void benchmark_strrfind_character(ankerl::nanobench::Bench& bench) {
    using ankerl::nanobench::doNotOptimizeAway;
    bench.minEpochIterations(450'000).run("character (229362)", []() BS_NOINLINE {
        auto result = bs::strrfind(strings::HOMOGENEOUS, 229362, 'b');
        doNotOptimizeAway(result);
    });
    bench.minEpochIterations(500'000).run("character (114681)", []() BS_NOINLINE {
        auto result = bs::strrfind(strings::HOMOGENEOUS, 114681, 'b');
        doNotOptimizeAway(result);
    });
    bench.minEpochIterations(65'000'000).run("character (1000)", []() BS_NOINLINE {
        auto result = bs::strrfind(strings::HOMOGENEOUS, 1000, 'b');
        doNotOptimizeAway(result);
    });
    bench.minEpochIterations(100'000'000).run("character (250)", []() BS_NOINLINE {
        auto result = bs::strrfind(strings::HOMOGENEOUS, 250, 'b');
        doNotOptimizeAway(result);
    });
    bench.minEpochIterations(500'000'000).run("character (60)", []() BS_NOINLINE {
        auto result = bs::strrfind(strings::HOMOGENEOUS, 60, 'b');
        doNotOptimizeAway(result);
    });
}

inline void benchmark_parse_u8(ankerl::nanobench::Bench& bench) {
    using ankerl::nanobench::doNotOptimizeAway;

    bench.minEpochIterations(350'000'000);
    for (const std::size_t sz : {1, 2, 3}) {
        bench.run(fmt::format("parse<uint8_t> ({})", sz), [&]() BS_NOINLINE {
            auto result = bs::parse<uint8_t>("123", sz);
            doNotOptimizeAway(result);
        });
    }
}
inline void benchmark_parse_u16(ankerl::nanobench::Bench& bench) {
    using ankerl::nanobench::doNotOptimizeAway;

    bench.minEpochIterations(300'000'000);
    for (const std::size_t sz : {1, 2, 3, 4, 5}) {
        bench.run(fmt::format("parse<uint16_t> ({})", sz), [&]() BS_NOINLINE {
            auto result = bs::parse<uint16_t>("12345", sz);
            doNotOptimizeAway(result);
        });
    }
}
inline void benchmark_parse_u32(ankerl::nanobench::Bench& bench) {
    using ankerl::nanobench::doNotOptimizeAway;

    bench.minEpochIterations(300'000'000);
    for (const std::size_t sz : {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}) {
        bench.run(fmt::format("parse<uint32_t> ({})", sz), [&]() BS_NOINLINE {
            auto result = bs::parse<uint32_t>("1234567890", sz);
            doNotOptimizeAway(result);
        });
    }
}
inline void benchmark_parse_u64(ankerl::nanobench::Bench& bench) {
    using ankerl::nanobench::doNotOptimizeAway;

    bench.minEpochIterations(350'000'000);
    for (const std::size_t sz : {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}) {
        bench.run(fmt::format("parse<uint64_t> ({})", sz), [&]() BS_NOINLINE {
            auto result = bs::parse<uint64_t>("12345678901234567890", sz);
            doNotOptimizeAway(result);
        });
    }
}

