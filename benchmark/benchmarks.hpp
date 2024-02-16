#pragma once

#include <array>
#include <utility>
#include <type_traits>
#include <vector>
#include <stdexcept>
#include <new>
#include <nanobench.h>

#include <betterstring/functions.hpp>
#include <betterstring/parsing.hpp>

#include <fmt/format.h>

#include "add_benchmark_macro.hpp"

ADD_BENCHMARK("strrfind_ch") {
    bench.title("bs::strrfind (character)");

    std::vector<char> homogeneous_string(1 << 20, 'a');
    for (std::size_t i = 0; i <= 20; ++i) {
        const std::size_t string_len = 1 << i;
        if (string_len > homogeneous_string.size()) { throw std::logic_error("Sample length exceeds allocated capacity"); }

        bench.context("length", fmt::format("{}", string_len));
        bench.run(fmt::format("length {}", string_len), [&]() {
            auto result = bs::strrfind(homogeneous_string.data(), string_len, 'b');
            bench.doNotOptimizeAway(result);
        });
    }
}
ADD_BENCHMARK("strrfind_ch_aligned") {
    if (args.size() == 0) {
        fmt::println("pass the alignment size argument (first)");
        return;
    }
    const auto aligment_arg = bs::parse<std::size_t>(args[0].data(), args[0].size());
    if (aligment_arg.has_error()) {
        fmt::println("bad number formatting");
        return;
    }
    const std::align_val_t aligment{aligment_arg.value()};
    bench.title(fmt::format("bs::strrfind (character) (alignment={})", std::size_t(aligment)));

    const size_t full_string_size = 1 << 20;
    char* const string = (char*)::operator new[](full_string_size, aligment);
    bs::strfill(string, full_string_size, 'a');

    for (std::size_t i = 0; i <= 20; ++i) {
        const std::size_t string_len = 1 << i;

        bench.context("length", fmt::format("{}", string_len));
        bench.run(fmt::format("length {}", string_len), [&]() {
            auto result = bs::strrfind(string, string_len, 'b');
            bench.doNotOptimizeAway(result);
        });
    }

    ::operator delete[](string, aligment);
}

ADD_BENCHMARK("parse_u8") {
    for (std::size_t i = 0; i <= 3; ++i) {
        bench.run(fmt::format("parse<uint8_t> ({})", i), [&]() {
            auto result = bs::parse<uint8_t>("123", i);
            bench.doNotOptimizeAway(result);
        });
    }
}
ADD_BENCHMARK("parse_u16") {
    bench.title("bs::parse<uint16_t>");

    for (std::size_t i = 0; i <= 5; ++i) {
        bench.run(fmt::format("parse<uint16_t> ({})", i), [&]() {
            auto result = bs::parse<uint16_t>("12345", i);
            bench.doNotOptimizeAway(result);
        });
    }
}
ADD_BENCHMARK("parse_u32") {
    bench.title("bs::parse<uint32_t>");

    for (std::size_t i = 0; i <= 10; ++i) {
        bench.run(fmt::format("parse<uint32_t> ({})", i), [&]() {
            auto result = bs::parse<uint32_t>("1234567890", i);
            bench.doNotOptimizeAway(result);
        });
    }
}
ADD_BENCHMARK("parse_u64") {
    bench.title("bs::parse<uint64_t>");

    for (std::size_t i = 0; i <= 20; ++i) {
        bench.run(fmt::format("length {}", i), [&]() {
            auto result = bs::parse<uint64_t>("12345678901234567890", i);
            bench.doNotOptimizeAway(result);
        });
    }
}

ADD_BENCHMARK("strcount_ch") {
    bench.title("bs::strcount (character)");

    std::vector<char> homogeneous_string(1 << 21, 'a');
    for (std::size_t i = 0; i <= 21; ++i) {
        const std::size_t string_len = 1 << i;
        if (string_len > homogeneous_string.size()) { throw std::logic_error("Sample length exceeds allocated capacity"); }

        bench.context("length", fmt::format("{}", string_len));
        bench.run(fmt::format("length {}", string_len), [&]() {
            std::size_t result = bs::strcount(homogeneous_string.data(), string_len, 'a');
            // std::size_t result = std::count(homogeneous_string.data(), homogeneous_string.data() + string_len, 'a');
            bench.doNotOptimizeAway(result);
        });
    }
}

#if BS_COMP_MSVC
    #pragma function (strlen)
#endif

ADD_BENCHMARK("strlen") {
    bench.title("bs::strlen");

    std::vector<char> string((1 << 21) + 1, 'X');

    for (std::size_t i = 0; i <= 21; ++i) {
        const std::size_t string_len = 1 << i;

        string[string_len] = '\0';
        bench.context("length", fmt::format("{}", string_len));
        bench.run(fmt::format("length {}", string_len), [&]() {
            std::size_t result = bs::strlen(string.data());
            bench.doNotOptimizeAway(result);
        });
        string[string_len] = 'X';
    }
}
