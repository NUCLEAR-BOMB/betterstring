#pragma once

#include "../add_benchmark_macro.hpp"
#include <betterstring/functions.hpp>
#include <betterstring/parsing.hpp>
#include <fmt/format.h>

#include "../util.hpp"

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
    const size_t aligment{aligment_arg.value()};
    bench.title(fmt::format("bs::strrfind (character) (alignment={})", std::size_t(aligment)));

    const size_t full_string_size = 1 << 20;
    char* const string = aligned_new<char>(full_string_size, aligment);
    bs::strfill(string, full_string_size, 'a');

    for (std::size_t i = 0; i <= 20; ++i) {
        const std::size_t string_len = 1 << i;

        bench.context("length", fmt::format("{}", string_len));
        bench.run(fmt::format("length {}", string_len), [&]() {
            auto result = bs::strrfind(string, string_len, 'b');
            bench.doNotOptimizeAway(result);
        });
    }

    aligned_delete(string, full_string_size, aligment);
}

ADD_BENCHMARK("strcount_ch") {
    bench.title("bs::strcount (character)");
    using ankerl::nanobench::Rng;

    const std::size_t full_string_len = 1 << 21;
    char* const string = new char[full_string_len];

    Rng rng;
    for (std::size_t i = 0; i < full_string_len; i += 8) {
        const uint64_t tmp = rng();
        std::memcpy(string + i, &tmp, 8);
    }

    for (std::size_t i = 0; i <= 21; ++i) {
        const std::size_t string_len = 1 << i;

        bench.context("length", fmt::format("{}", string_len));
        bench.run(fmt::format("length {}", string_len), [&]() {
            std::size_t result = bs::strcount(string, string_len, 'a');
            bench.doNotOptimizeAway(result);
        });
    }

    delete[] string;
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

ADD_BENCHMARK("strlen_aligned") {
    if (args.size() == 0) {
        fmt::println("pass the alignment size argument (first)");
        return;
    }
    const auto aligment_arg = bs::parse<std::size_t>(args[0].data(), args[0].size());
    if (aligment_arg.has_error()) {
        fmt::println("bad number formatting");
        return;
    }
    const std::size_t aligment{aligment_arg.value()};
    bench.title(fmt::format("bs::strlen (alignment={})", aligment));

    const size_t full_string_size = (1 << 21) + 1;
    char* const string = aligned_new<char>(full_string_size, aligment);
    bs::strfill(string, full_string_size, 'X');

    for (std::size_t i = 0; i <= 21; ++i) {
        const std::size_t string_len = 1 << i;

        string[string_len] = '\0';
        bench.context("length", fmt::format("{}", string_len));
        bench.run(fmt::format("length {}", string_len), [&]() {
            std::size_t result = bs::strlen(string);
            bench.doNotOptimizeAway(result);
        });
        string[string_len] = 'X';
    }

    aligned_delete(string, full_string_size, aligment);
}

ADD_BENCHMARK("strfindn_ch") {
    bench.title("bs::strfindn (character)");

    std::vector<char> string((1 << 21) + 1, 'X');

    for (std::size_t i = 0; i <= 21; ++i) {
        const std::size_t string_len = 1 << i;

        string[string_len - 1] = 'Y';
        bench.context("length", fmt::format("{}", string_len));
        bench.run(fmt::format("length {}", string_len), [&]() {
            char* result = bs::strfindn(string.data(), string_len, 'X');
            bench.doNotOptimizeAway(result);
        });
        string[string_len - 1] = 'X';
    }
}

ADD_BENCHMARK("strfirstof") {
    using ankerl::nanobench::Rng;
    if (args.size() == 0) {
        fmt::println("pass the character sequence length argument (first)");
        return;
    }
    const auto char_seq_len = bs::parse<std::size_t>(args[0].data(), args[0].size());
    if (char_seq_len.has_error()) {
        fmt::println("bad number formatting");
        return;
    }

    bench.title(fmt::format("bs::strfirst_of (seq length={})", char_seq_len.value()));

    std::vector<char> string(1 << 21, 'X');

    std::vector<char> char_seq(char_seq_len.value());

    Rng rng;
    for (char& ch : char_seq) {
    random_again:
        ch = rng.bounded(256);
        if (ch == string[0]) { goto random_again; }
    }

    for (std::size_t i = 0; i <= 21; ++i) {
        const std::size_t string_len = 1 << i;

        string[string_len - 1] = 'Y';
        bench.context("length", fmt::format("{}", string_len));
        bench.run(fmt::format("length {}", string_len), [&]() {
            char* result = bs::strfirstof(string.data(), string_len, char_seq.data(), char_seq.size());
            bench.doNotOptimizeAway(result);
        });
        string[string_len - 1] = 'X';
    }
}


