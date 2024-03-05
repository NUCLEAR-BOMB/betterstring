#pragma once

#include "../add_benchmark_macro.hpp"
#include <fmt/format.h>
#include <betterstring/parsing.hpp>
#include <charconv>

ADD_BENCHMARK("parse_u8") {
    bench.title("bs::parse<uint8_t>");

    const char* const sample_str = "123";
    for (std::size_t i = 0; i <= 3; ++i) {
        bench.context("length", fmt::format("{}", i));
        bench.run(fmt::format("length {}", i), [&]() {
            auto result = bs::parse<uint8_t>(sample_str, i);
            bench.doNotOptimizeAway(result);
        });
    }
}
ADD_BENCHMARK("parse_u16") {
    using namespace std::chrono_literals;
    bench.title("bs::parse<uint16_t>");
    bench.minEpochTime(300ms);

    const char* const sample_str = "12345";
    for (std::size_t i = 0; i <= 5; ++i) {
        bench.context("length", fmt::format("{}", i));
        bench.run(fmt::format("length {}", i), [&]() {
            auto result = bs::parse<uint16_t>(sample_str, i);
            bench.doNotOptimizeAway(result);
        });
    }
}
ADD_BENCHMARK("parse_u32") {
    bench.title("bs::parse<uint32_t>");

    const char* const sample_str = "1234567890";
    for (std::size_t i = 0; i <= 10; ++i) {
        bench.context("length", fmt::format("{}", i));
        bench.run(fmt::format("length {}", i), [&]() {
            auto result = bs::parse<uint32_t>(sample_str, i);
            bench.doNotOptimizeAway(result);
        });
    }
}
ADD_BENCHMARK("parse_u64") {
    bench.title("bs::parse<uint64_t>");

    const char* const sample_str = "12345678901234567890";
    for (std::size_t i = 0; i <= 20; ++i) {
        bench.context("length", fmt::format("{}", i));
        bench.run(fmt::format("length {}", i), [&]() {
            auto result = bs::parse<uint64_t>(sample_str, i);
            bench.doNotOptimizeAway(result);
        });
    }
}
ADD_BENCHMARK("from_chars") {
    bench.title("std::from_chars");

    const char* const sample_str = "12345678901234567890";
    for (std::size_t i = 0; i <= 20; ++i) {
        bench.context("length", fmt::format("{}", i));
        bench.run(fmt::format("length {}", i), [&]() {
            uint64_t result{};
            auto err = std::from_chars(sample_str, sample_str + i, result);
            bench.doNotOptimizeAway(err);
            bench.doNotOptimizeAway(result);
        });
    }
}
ADD_BENCHMARK("strtoull") {
    bench.title("std::strtoull");

    const char* const sample_str = "12345678901234567890";
    char buffer[21]{};
    for (std::size_t i = 0; i <= 20; ++i) {
        buffer[i] = sample_str[i];

        bench.context("length", fmt::format("{}", i));
        bench.run(fmt::format("length {}", i), [&]() {
            char* end = nullptr;
            uint64_t result = std::strtoull(buffer, &end, 10);
            bench.doNotOptimizeAway(result);
            bench.doNotOptimizeAway(end);
        });
    }
}
