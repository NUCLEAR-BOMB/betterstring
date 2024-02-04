#pragma once

#include <array>
#include <utility>
#include <type_traits>
#include <nanobench.h>

#include <betterstring/functions.hpp>
#include <betterstring/parsing.hpp>

#include <fmt/format.h>

#include "samples.hpp"

template<class T, std::size_t N, std::size_t... I>
constexpr auto to_array_impl(T (&&a)[N], std::index_sequence<I...>)
    -> std::array<std::remove_cv_t<T>, N> {
    return {{std::move(a[I])...}};
}
 
template<class T, std::size_t N>
constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&&a)[N]) {
    return to_array_impl(std::move(a), std::make_index_sequence<N>{});
}

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
    
    const auto samples = to_array<std::size_t>({
        229363,
        114681,
        80000,
        40000,
        20000,
        9000,
        8192,
        4200,
        4096,
        2048,
        1024,
        512,
        384,
        256,
        192,
        128,
        96,
        64,
        48,
        32,
        20,
    });
    using namespace std::chrono_literals;
    bench.minEpochTime(200ms).minEpochIterations(500);
    for (const auto& sample : samples) {
        bench.run(fmt::format("character ({} lenght)", sample), [&]() {
            auto result = bs::strrfind(strings::HOMOGENEOUS, sample, 'b');
            doNotOptimizeAway(result);
        });
    }

#if 0
    bench.minEpochIterations(320'000).run("character (229362)", []() BS_NOINLINE {
        auto result = bs::strrfind(strings::HOMOGENEOUS, 229362, 'b');
        doNotOptimizeAway(result);
    });
    bench.minEpochIterations(400'000).run("character (114681)", []() BS_NOINLINE {
        auto result = bs::strrfind(strings::HOMOGENEOUS, 114681, 'b');
        doNotOptimizeAway(result);
    });
    bench.minEpochIterations(60'000'000).run("character (1000)", []() BS_NOINLINE {
        auto result = bs::strrfind(strings::HOMOGENEOUS, 1000, 'b');
        doNotOptimizeAway(result);
    });
    bench.minEpochIterations(80'000'000).run("character (250)", []() BS_NOINLINE {
        auto result = bs::strrfind(strings::HOMOGENEOUS, 250, 'b');
        doNotOptimizeAway(result);
    });
    bench.minEpochIterations(110'000'000).run("character (60)", []() BS_NOINLINE {
        auto result = bs::strrfind(strings::HOMOGENEOUS, 60, 'b');
        doNotOptimizeAway(result);
    });
    bench.minEpochIterations(200'000'000).run("character (32)", []() BS_NOINLINE {
        auto result = bs::strrfind(strings::HOMOGENEOUS, 32, 'b');
        doNotOptimizeAway(result);
    });
    bench.minEpochIterations(200'000'000).run("character (20)", []() BS_NOINLINE {
        auto result = bs::strrfind(strings::HOMOGENEOUS, 20, 'b');
        doNotOptimizeAway(result);
    });
#endif
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

