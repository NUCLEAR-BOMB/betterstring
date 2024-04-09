
// Copyright 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>

#include <betterstring/unicode.hpp>

namespace {

using bs::u8char_t;

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

}
