#pragma once

#include "../add_benchmark_macro.hpp"
#include <betterstring/functions.hpp>
#include <betterstring/parsing.hpp>
#include <fmt/format.h>

#include "../util.hpp"

static std::vector<uint64_t> generate_length_sequence(uint64_t max_pow2) {
    std::vector<uint64_t> out;
    for (std::size_t i = 1; i <= max_pow2; ++i) {
        out.emplace_back(((1 << i) + (1 << (i - 1))) / 2);
        out.emplace_back(1 << i);
    }
    return out;
}

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
    std::generate_n(string, full_string_len, [rng = Rng{}]() mutable -> char { return rng(); });

    const char count_for = Rng{}();

    const std::vector<uint64_t> string_lengths_sequence = generate_length_sequence(21);
    for (auto [string_len, index] : enumerate{string_lengths_sequence}) {
        bench.context("length", fmt::format("{}", string_len));
        bench.run(fmt::format("length {} ({}/{})", string_len, index + 1, string_lengths_sequence.size()), [&]() {
            std::size_t result = bs::strcount(string, string_len, count_for);
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

    bench.title(fmt::format("bs::strfirstof (seq length={})", char_seq_len.value()));

    std::vector<char> string(1 << 21, 'X');

    std::vector<char> char_seq(char_seq_len.value());

    Rng rng;
    for (char& ch : char_seq) {
    random_again:
        ch = rng.bounded(256);
        if (ch == string[0]) { goto random_again; }
    }

    const std::vector<uint64_t> string_lengths_sequence = generate_length_sequence(21);

    for (auto [string_len, index] : enumerate{string_lengths_sequence}) {
        string[string_len - 1] = char_seq[0];
        bench.context("length", fmt::format("{}", string_len));
        bench.run(fmt::format("length {} ({}/{})", string_len, index + 1, string_lengths_sequence.size()),
        [&]() {
            char* result = bs::strfirstof(string.data(), string_len, char_seq.data(), char_seq.size());
            bench.doNotOptimizeAway(result);
        });
        string[string_len - 1] = 'X';
    }
}

ADD_BENCHMARK("strfirstnof") {
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

    bench.title(fmt::format("bs::strfirstnof (seq length={})", char_seq_len.value()));

    std::vector<char> string(1 << 21, 'X');

    const std::vector<char> char_seq(char_seq_len.value(), 'X');

    const std::vector<uint64_t> string_lengths_sequence = generate_length_sequence(21);
    // const std::vector<uint64_t> string_lengths_sequence{24, 25, 26, 27, 28, 29, 30, 31, 32};
    for (auto [string_len, index] : enumerate{string_lengths_sequence}) {
        string[string_len - 1] = 'Y';
        bench.context("length", fmt::format("{}", string_len));
        bench.run(fmt::format("length {} ({}/{})", string_len, index + 1, string_lengths_sequence.size()),
        [&]() {
            char* result = bs::strfirstnof(string.data(), string_len, char_seq.data(), char_seq.size());
            bench.doNotOptimizeAway(result);
        });
        string[string_len - 1] = 'X';
    }
}


