
// Copyright 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <betterstring/unicode.hpp>

namespace {

using bs::u8char_t;

constexpr u8char_t operator ""_u8(const unsigned long long x) noexcept {
    return u8char_t(x);
}

TEST_CASE("utf8 length", "[unicode]") {
    CHECK(bs::utf8::sequence_length(127) == 1);
    CHECK(bs::utf8::sequence_length('\x0') == 1);
    CHECK(bs::utf8::sequence_length('\x0') == 1);
    
    CHECK(bs::utf8::sequence_length(0b1100'0000) == 2);
    CHECK(bs::utf8::sequence_length(0b1101'1111) == 2);

    CHECK(bs::utf8::sequence_length(0b1110'0000) == 3);
    CHECK(bs::utf8::sequence_length(0b1110'1111) == 3);

    CHECK(bs::utf8::sequence_length(0b1111'0000) == 4);
    CHECK(bs::utf8::sequence_length(0b1111'0111) == 4);

    CHECK(bs::utf8::sequence_length(0b1111'1000) == bs::utf_error::bad_lead);
    CHECK(bs::utf8::sequence_length(0b1111'1100) == bs::utf_error::bad_lead);
    CHECK(bs::utf8::sequence_length(0b1111'1110) == bs::utf_error::bad_lead);
    CHECK(bs::utf8::sequence_length(0b1111'1111) == bs::utf_error::bad_lead);
    CHECK(bs::utf8::sequence_length(0b1000'0000) == bs::utf_error::bad_lead);
}

TEST_CASE("utf8::append", "[unicode]") {
    std::array<u8char_t, 1> buffer1{};
    bs::utf8::append(buffer1.data(), U'$');
    CHECK(buffer1 == std::array{0b00100100_u8});

    std::array<u8char_t, 2> buffer2{};
    bs::utf8::append(buffer2.data(), U'£');
    CHECK(buffer2 == std::array{0b11000010_u8, 0b10100011_u8});

    std::array<u8char_t, 2> buffer3{};
    bs::utf8::append(buffer3.data(), U'И');
    CHECK(buffer3 == std::array{0b11010000_u8, 0b10011000_u8});

    std::array<u8char_t, 3> buffer4{};
    bs::utf8::append(buffer4.data(), U'ह');
    CHECK(buffer4 == std::array{0b11100000_u8, 0b10100100_u8, 0b10111001_u8});

    std::array<u8char_t, 3> buffer5{};
    bs::utf8::append(buffer5.data(), U'€');
    CHECK(buffer5 == std::array{0b11100010_u8, 0b10000010_u8, 0b10101100_u8});

    std::array<u8char_t, 3> buffer6{};
    bs::utf8::append(buffer6.data(), U'한');
    CHECK(buffer6 == std::array{0b11101101_u8, 0b10010101_u8, 0b10011100_u8});

    std::array<u8char_t, 4> buffer7{};
    bs::utf8::append(buffer7.data(), U'𐍈');
    CHECK(buffer7 == std::array{0b11110000_u8, 0b10010000_u8, 0b10001101_u8, 0b10001000_u8});

    std::array<u8char_t, 4> buffer8{};
    bs::utf8::append(buffer8.data(), U'\U001096B3');
    CHECK(buffer8 == std::array{0b11110100_u8, 0b10001001_u8, 0b10011010_u8, 0b10110011_u8});

    std::array<u8char_t, 4> buffer9{};
    bs::utf8::append(buffer9.data(), U'\U0010FFFF');
    CHECK(buffer9 == std::array{0b11110100_u8, 0b10001111_u8, 0b10111111_u8, 0b10111111_u8});

    std::array<u8char_t, 1> buffer10{};
    bs::utf8::append(buffer10.data(), U'\U00000000');
    CHECK(buffer10 == std::array{0b0000'0000_u8});
}

}
