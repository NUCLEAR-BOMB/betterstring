
// Copyright 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>

#include "stacktrace.hpp"
#include <betterstring/safe_functions.hpp>

namespace Catch {
    template<>
    struct StringMaker<bs::safe::error> {
        static std::string convert(const bs::safe::error value) {
            return std::string{value.error_message()};
        }
    };
    template<>
    struct StringMaker<bs::safe::errorc> {
        static std::string convert(const bs::safe::errorc value) {
            return std::to_string(static_cast<uint32_t>(value));
        }
    };
}

namespace {

TEST_CASE("bs::safe::strcopy", "[safe functions]") {
    char* const char_nullptr = nullptr;

    char buffer[20]{};
    auto err1 = bs::safe::strcopy(buffer, 20, "hello world", 11);
    CHECK(err1 == bs::safe::errorc{});

    auto err2 = bs::safe::strcopy(buffer, 20, "long long long long string", 26);
    CHECK(err2 == bs::safe::errorc::not_enough_space);
    CHECK(err2.error_message() != "");

    auto err3 = bs::safe::strcopy(char_nullptr, 0, char_nullptr, 0);
    CHECK(err3 == bs::safe::errorc{});

    auto err4 = bs::safe::strcopy(buffer, 20, char_nullptr, 0);
    CHECK(err4 == bs::safe::errorc{});

    auto err5 = bs::safe::strcopy(buffer, 20, char_nullptr, 1);
    CHECK(err5 == bs::safe::errorc::src_null_pointer);
    CHECK(err5.error_message() != "");

    auto err6 = bs::safe::strcopy(char_nullptr, 1, "test", 4);
    CHECK(err6 == bs::safe::errorc::dest_null_pointer);
    CHECK(err6.error_message() != "");

    auto err7 = bs::safe::strcopy(buffer, 20, buffer + 5, 5);
    CHECK(err7 == bs::safe::errorc::overlapping);

    auto err8 = bs::safe::strcopy(buffer, 10, buffer + 10, 10);
    CHECK(err8 == bs::safe::errorc{});
}

TEST_CASE("bs::safe::strmove", "[safe functions]") {
    char* const char_nullptr = nullptr;

    char buffer[20]{};
    auto err1 = bs::safe::strmove(buffer, 20, "hello world", 11);
    CHECK(err1 == bs::safe::errorc{});

    auto err2 = bs::safe::strmove(buffer, 20, "long long long long string", 26);
    CHECK(err2 == bs::safe::errorc::not_enough_space);
    CHECK(err2.error_message() != "");

    auto err3 = bs::safe::strmove(char_nullptr, 0, char_nullptr, 0);
    CHECK(err3 == bs::safe::errorc{});

    auto err4 = bs::safe::strmove(buffer, 20, char_nullptr, 0);
    CHECK(err4 == bs::safe::errorc{});

    auto err5 = bs::safe::strmove(buffer, 20, char_nullptr, 1);
    CHECK(err5 == bs::safe::errorc::src_null_pointer);
    CHECK(err5.error_message() != "");

    auto err6 = bs::safe::strmove(char_nullptr, 1, "test", 4);
    CHECK(err6 == bs::safe::errorc::dest_null_pointer);
    CHECK(err6.error_message() != "");

    auto err8 = bs::safe::strmove(buffer, 20, buffer + 10, 10);
    CHECK(err8 == bs::safe::errorc{});
}

}
