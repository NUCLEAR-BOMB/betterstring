#pragma once

#include <array>
#include <utility>
#include <type_traits>
#include <vector>
#include <stdexcept>
#include <nanobench.h>

#include <betterstring/functions.hpp>
#include <betterstring/parsing.hpp>

#include <fmt/format.h>

template<class T, std::size_t N, std::size_t... I>
constexpr auto to_array_impl(T (&&a)[N], std::index_sequence<I...>)
    -> std::array<std::remove_cv_t<T>, N> {
    return {{std::move(a[I])...}};
}
 
template<class T, std::size_t N>
constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&&a)[N]) {
    return to_array_impl(std::move(a), std::make_index_sequence<N>{});
}

inline void benchmark_strrfind_character(ankerl::nanobench::Bench& bench) {
    std::vector<char> homogeneous_string(1 << 20, 'a');

    bench.title("bs::strrfind (character)");
    for (std::size_t i = 0; i <= 20; ++i) {
        const std::size_t string_len = 1 << i;
        if (string_len > homogeneous_string.size()) { throw std::logic_error("Sample length exceeds allocated capacity"); }

        bench.run(fmt::format("length {}", string_len), [&]() {
            auto result = bs::strrfind(homogeneous_string.data(), string_len, 'b');
            bench.doNotOptimizeAway(result);
        });
    }
}

inline void benchmark_parse_u8(ankerl::nanobench::Bench& bench) {
    for (std::size_t i = 0; i <= 3; ++i) {
        bench.run(fmt::format("parse<uint8_t> ({})", i), [&]() {
            auto result = bs::parse<uint8_t>("123", i);
            bench.doNotOptimizeAway(result);
        });
    }
}
inline void benchmark_parse_u16(ankerl::nanobench::Bench& bench) {
    bench.title("bs::parse<uint16_t>");

    for (std::size_t i = 0; i <= 5; ++i) {
        bench.run(fmt::format("parse<uint16_t> ({})", i), [&]() {
            auto result = bs::parse<uint16_t>("12345", i);
            bench.doNotOptimizeAway(result);
        });
    }
}
inline void benchmark_parse_u32(ankerl::nanobench::Bench& bench) {
    bench.title("bs::parse<uint32_t>");

    for (std::size_t i = 0; i <= 10; ++i) {
        bench.run(fmt::format("parse<uint32_t> ({})", i), [&]() {
            auto result = bs::parse<uint32_t>("1234567890", i);
            bench.doNotOptimizeAway(result);
        });
    }
}
inline void benchmark_parse_u64(ankerl::nanobench::Bench& bench) {
    bench.title("bs::parse<uint64_t>");

    for (std::size_t i = 0; i <= 20; ++i) {
        bench.run(fmt::format("length {}", i), [&]() {
            auto result = bs::parse<uint64_t>("12345678901234567890", i);
            bench.doNotOptimizeAway(result);
        });
    }
}

inline void benchmark_strcount_ch(ankerl::nanobench::Bench& bench) {
    bench.title("bs::strcount (character)");

    std::vector<char> homogeneous_string(1 << 21, 'a');
    for (std::size_t i = 0; i <= 21; ++i) {
        const std::size_t string_len = 1 << i;
        if (string_len > homogeneous_string.size()) { throw std::logic_error("Sample length exceeds allocated capacity"); }

        bench.run(fmt::format("length {}", string_len), [&]() {
            std::size_t result = bs::strcount(homogeneous_string.data(), string_len, 'a');
            bench.doNotOptimizeAway(result);
        });
    }
}

