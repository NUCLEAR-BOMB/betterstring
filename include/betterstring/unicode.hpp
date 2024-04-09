
// Copyright 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <betterstring/detail/preprocessor.hpp>
#include <cstddef>
#include <cstdint>

namespace bs {

#if BS_HAS_CHAR8_T
using u8char_t = char8_t;
#else
using u8char_t = char;
#endif

enum class utf_error : uint32_t {
    ok = 0,
    bad_lead = 1,

};

template<class T>
class utf_result {
public:
    using value_type = T;
    using error_type = utf_error;

    constexpr utf_result(const utf_error err) noexcept : val{0}, err{err} {}
    constexpr utf_result(const T result) noexcept : val{result}, err{utf_error::ok} {}

    constexpr T value() const noexcept {
        BS_VERIFY(err == utf_error::ok, "No valid utf result");
        return val;
    }
    constexpr utf_error error() const noexcept {
        return err;
    }

    constexpr bool has_error() const noexcept { return err != utf_error::ok; }
    explicit constexpr operator bool() const noexcept { return err == utf_error::ok; }

    friend constexpr bool operator==(const utf_result left, const utf_result right) noexcept {
        if (left.err == right.err) { return true; }
        return left.val == right.val;
    }
    friend constexpr bool operator!=(const utf_result left, const utf_result right) noexcept {
        return !(left == right);
    }

private:
    T val;
    utf_error err;
};

template<class T>
class utf_traits;

template<>
class utf_traits<u8char_t> {
public:
    using size_type = std::size_t;

    static constexpr utf_result<uint8_t> length(const uint8_t ch) noexcept {
        if ((ch & 0b1000'0000) == 0b0000'0000) { return 1; }
        if ((ch & 0b1110'0000) == 0b1100'0000) { return 2; }
        if ((ch & 0b1111'0000) == 0b1110'0000) { return 3; }
        if ((ch & 0b1111'1000) == 0b1111'0000) { return 3; }
        return utf_error::bad_lead;
    }
};

}


